#include "pchAOS_Base.hpp"
#include "AOSContextManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"

#ifdef __DEBUG_DUMP__
void AOSContextManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSContextManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Freestore.size()=" << m_Freestore.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_InUse.size()=" << m_InUse.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_InUse={" << std::endl;
  CONTEXT_INUSE::const_iterator citU = m_InUse.begin();
  while(citU != m_InUse.end())
  {
    (*citU).first->debugDump(os, indent+2);
    ++citU;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_History.size()=" << m_History.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_History={" << std::endl;
  CONTEXT_HISTORY::const_iterator citH = m_History.begin();
  while(citH != m_History.end())
  {
    (*citH)->debugDump(os, indent+2);
    ++citH;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSContextManager::getClass() const
{
  static const AString CLASS("AOSContextManager");
  return CLASS;
}

void AOSContextManager::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  addProperty(eBase, "history_max_size", AString::fromSize_t(m_HistoryMaxSize));
  addProperty(eBase, "freestore_max_size", AString::fromSize_t(m_FreestoreMaxSize));

  AXmlElement& eInUse = eBase.addElement("object");
  eInUse.addAttribute("name", "InUse");
  eInUse.addAttribute("size", AString::fromSize_t(m_InUse.size()));
  CONTEXT_INUSE::const_iterator citU = m_InUse.begin();
  if (citU != m_InUse.end())
  {
    ALock lock(m_InUseSync);
    while(citU != m_InUse.end())
    {
      AXmlElement& eProp = addProperty(eInUse, ASW("context",7), (*citU).first->useEventVisitor(), AXmlData::CDataDirect);
      eProp.addAttribute(ASW("errors",6), AString::fromSize_t((*citU).first->useEventVisitor().getErrorCount()));
      eProp.addElement(ASW("url",3), (*citU).first->useEventVisitor().useName(), AXmlData::CDataDirect);
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
      AXmlElement& eProp = addProperty(eHistory, ASW("context",7), (*citH)->useEventVisitor(), AXmlData::CDataDirect);
      eProp.addAttribute(ASW("errors",6), AString::fromSize_t((*citH)->useEventVisitor().getErrorCount()));
      eProp.addElement(ASW("url",3), (*citH)->useEventVisitor().useName(), AXmlData::CDataDirect);
      ++citH;
    }
  }
}

AOSContextManager::AOSContextManager(AOSServices& services) :
  m_Services(services)
{
  m_HistoryMaxSize = services.useConfiguration().getInt("/config/server/context-manager/history-maxsize", 100);
  m_FreestoreMaxSize = services.useConfiguration().getInt("/config/server/context-manager/freestore-maxsize", 50);

  registerAdminObject(m_Services.useAdminRegistry());
}

AOSContextManager::~AOSContextManager()
{
  CONTEXT_FREESTORE::iterator itF = m_Freestore.begin();
  while(itF != m_Freestore.end())
  {
    delete (*itF);
    ++itF;
  }

  CONTEXT_HISTORY::iterator itH = m_History.begin();
  while(itH != m_History.end())
  {
    delete (*itH);
    ++itH;
  }

  CONTEXT_INUSE::iterator itU = m_InUse.begin();
  while(itU != m_InUse.end())
  {
    delete (*itU).first;
    ++itU;
  }
}

AOSContext *AOSContextManager::allocate(AFile_Socket *pSocket)
{
  AASSERT(this, pSocket);

  AOSContext *p = new AOSContext(pSocket, m_Services);

  {
    ALock lock(m_InUseSync);
    AASSERT(this, p && m_InUse.end() == m_InUse.find(p));
    m_InUse[p] = 1;
  }

  ARope rope("AOSContextManager::allocate[",28);
  rope.append(AString::fromPointer(p));
  rope.append(", pFile=0x",10);
  rope.append(AString::fromPointer(pSocket));
  rope.append(']');
  p->setExecutionState(rope);

  return p;
}

void AOSContextManager::deallocate(AOSContext *p)
{
  //a_Close it if still open
  if (p->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
  {
    p->setExecutionState(ASW("AOSContextManager::deallocate: Socket error detected",52));
  }
  else
  {
    p->setExecutionState(ASW("AOSContextManager::deallocate: Closing socket connection",56));
    p->useSocket().close();
  }

  if (p)
  {
    ALock lock(m_InUseSync);
    CONTEXT_INUSE::iterator it = m_InUse.find(p);
    AASSERT(this, p && m_InUse.end() != it);
    
    m_InUse.erase(it);
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
      //a_Remove last
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