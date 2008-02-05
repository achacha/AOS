#include "pchAOS_Base.hpp"
#include "AOSContextManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"
#include "AOSContextQueueInterface.hpp"

void AOSContextManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSContextManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Queues={" << std::endl;
  for (size_t i=0; i < m_Queues.size(); ++i)
  {
    AOSContextQueueInterface *pQueue = m_Queues.at(i);
    if (pQueue)
    {
      ADebugDumpable::indent(os, indent+2) << "[" << i << "]={" << std::endl;
      pQueue->debugDump(os, indent+3);
      ADebugDumpable::indent(os, indent+2) << "]" << std::endl;
    }
    else
      ADebugDumpable::indent(os, indent+2) << "[" << i << "]=NULL" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Freestore.size()=" << m_Freestore.size() << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_InUse.size()=" << m_InUse.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_InUse={" << std::endl;
  {
    ALock lock(const_cast<ASync_CriticalSection *>(&m_InUseSync));
    for (CONTEXT_INUSE::const_iterator citU = m_InUse.begin(); citU != m_InUse.end(); ++citU)
    {
      (*citU).first->debugDump(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_History.size()=" << m_History.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_History={" << std::endl;
  {
    ALock lock(const_cast<ASync_CriticalSection *>(&m_HistorySync));
    for (CONTEXT_HISTORY::const_iterator citH = m_History.begin(); citH != m_History.end(); ++citH)
    {
      (*citH)->debugDump(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSContextManager::getClass() const
{
  static const AString CLASS("AOSContextManager");
  return CLASS;
}

void AOSContextManager::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  for (size_t i=0; i < m_Queues.size(); ++i)
  {
    AOSContextQueueInterface *pQueue = m_Queues.at(i);
    if (pQueue)
      adminAddProperty(eBase, AString::fromSize_t(i), pQueue->getClass());
    else
      adminAddProperty(eBase, AString::fromSize_t(i), AConstant::ASTRING_NULL);
  }

  adminAddProperty(eBase, "history_max_size", AString::fromSize_t(m_HistoryMaxSize));
  adminAddProperty(eBase, "freestore_max_size", AString::fromSize_t(m_FreestoreMaxSize));

  AXmlElement& eInUse = eBase.addElement("object");
  eInUse.addAttribute("name", "InUse");
  eInUse.addAttribute("size", AString::fromSize_t(m_InUse.size()));
  CONTEXT_INUSE::const_iterator citU = m_InUse.begin();
  if (citU != m_InUse.end())
  {
    ALock lock(m_InUseSync);
    while(citU != m_InUse.end())
    {
      AXmlElement& eProp = adminAddProperty(eInUse, ASW("context",7), (*citU).first->useEventVisitor(), AXmlElement::ENC_CDATADIRECT);
      eProp.addAttribute(ASW("errors",6), AString::fromSize_t((*citU).first->useEventVisitor().getErrorCount()));
      eProp.addElement(ASW("url",3), (*citU).first->useEventVisitor().useName(), AXmlElement::ENC_CDATADIRECT);
      ++citU;
    }
  }

  AXmlElement& eHistory = eBase.addElement("object").addAttribute("name", "History");
  CONTEXT_HISTORY::const_iterator citH = m_History.begin();
  if (citH != m_History.end())
  {
    ALock lock(m_HistorySync);
    while(citH != m_History.end())
    {
      AXmlElement& eProp = adminAddProperty(eHistory, ASW("context",7), (*citH)->useEventVisitor(), AXmlElement::ENC_CDATADIRECT);
      eProp.addAttribute(ASW("errors",6), AString::fromSize_t((*citH)->useEventVisitor().getErrorCount()));
      eProp.addElement(ASW("url",3), (*citH)->useEventVisitor().useName(), AXmlElement::ENC_CDATADIRECT);
      ++citH;
    }
  }
}

AOSContextManager::AOSContextManager(AOSServices& services) :
  m_Services(services)
{
  m_HistoryMaxSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/history-maxsize", 100);
  m_FreestoreMaxSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/freestore-maxsize", 50);

  m_Queues.resize(AOSContextManager::STATE_TERMINATE+1, NULL);

  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSContextManager::~AOSContextManager()
{
  try
  {
    for (CONTEXT_FREESTORE::iterator itF = m_Freestore.begin(); itF != m_Freestore.end(); ++itF)
    {
      delete (*itF);
    }

    for (CONTEXT_HISTORY::iterator itH = m_History.begin(); itH != m_History.end(); ++itH)
    {
      delete (*itH);
      
    }
    
    for (CONTEXT_INUSE::iterator itU = m_InUse.begin(); itU != m_InUse.end(); ++itU)
    {
      delete (*itU).first;
    }

    for (QUEUES::iterator itQ = m_Queues.begin(); itQ != m_Queues.end(); ++itQ)
    {
      delete (*itQ);
    }
  }
  catch(...) {}
}

AOSContext *AOSContextManager::allocate(AFile_Socket *pSocket)
{
  AASSERT(this, pSocket);

  AOSContext *p = NULL;
  {
    ALock lock(m_FreestoreSync);
    if (m_Freestore.size() > 0)
    {
      p = m_Freestore.back();
      m_Freestore.pop_back();

      ARope rope("AOSContextManager::allocate[",28);
      rope.append(AString::fromPointer(p));
      rope.append(", pFile=0x",10);
      rope.append(AString::fromPointer(pSocket));
      rope.append("] freestore",11);
      p->setExecutionState(rope);
    }
    else
    {
      p = new AOSContext(pSocket, m_Services);

      ARope rope("AOSContextManager::allocate[",28);
      rope.append(AString::fromPointer(p));
      rope.append(", pFile=0x",10);
      rope.append(AString::fromPointer(pSocket));
      rope.append("] new create",12);
      p->setExecutionState(rope);
    }
  }

  {
    ALock lock(m_InUseSync);
    AASSERT(this, p && m_InUse.end() == m_InUse.find(p));
    m_InUse[p] = 1;
  }

  return p;
}

void AOSContextManager::deallocate(AOSContext *p)
{

  if (p)
  {
    p->finalize();
    
    {
      ALock lock(m_InUseSync);
      CONTEXT_INUSE::iterator it = m_InUse.find(p);
      AASSERT(this, p && m_InUse.end() != it);
    
      m_InUse.erase(it);
    }
  }
  else
    return;

  AOSContext *pFree = NULL;

  //a_Log
  ARope rope("AOSContextManager::deallocate[",30);
  rope.append(AString::fromPointer(p));
  rope.append(']');
  p->setExecutionState(rope);
  
  //a_Stop event visitor timer and clear current event, then log to log
  p->useEventVisitor().reset();
  if (p->useEventVisitor().getErrorCount() > 0)
  {
    m_Services.useLog().add(p->useEventVisitor(), ALog::CRITICAL_ERROR);
  }
  else
  {
    m_Services.useLog().add(p->useEventVisitor(), ALog::DEBUG);
  }

  //a_Add to history
  {
    ALock lock(m_HistorySync);
    if (m_History.size() >= m_HistoryMaxSize)
    {
      //a_Remove last and put into free store
      pFree = m_History.back();
      m_History.pop_back();
    }
    m_History.push_front(p);
  }

  //a_Freestore
  if (pFree)
  {
    //a_Check if freestore has room
    {
      ALock lock(m_FreestoreSync);
      if (m_Freestore.size() < m_FreestoreMaxSize)
      {
        pFree->clear();
        m_Freestore.push_front(pFree);
        pFree = NULL;
      }
      else
      {
        //a_Release it we do not need it
        delete pFree;
      }
    }
  }
}

void AOSContextManager::setQueueForState(AOSContextManager::ContextQueueState state, AOSContextQueueInterface *pQueue)
{
  AASSERT_EX(this, !m_Queues.at(state), ASWNL("State already associated with a queue, deleting and overwriting"));
  m_Queues[state] = pQueue;
}

void AOSContextManager::changeQueueState(AOSContextManager::ContextQueueState state, AOSContext **ppContext)
{
  AOSContextQueueInterface *pQueue = m_Queues.at(state);
  if (pQueue)
  {
    pQueue->add(*ppContext);
  }
  else
  {
    //a_No such queue, assume terminate
    AOSContextManager::deallocate(*ppContext);
  }
  *ppContext = NULL;
}
