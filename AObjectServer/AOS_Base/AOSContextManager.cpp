/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSContextQueueInterface.hpp"

void AOSContextManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

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

  ADebugDumpable::indent(os, indent+1) << "m_InUse.size()=" << m_InUse.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_InUse={" << std::endl;
  {
    ALock lock(const_cast<ASync_CriticalSection *>(&m_InUseSync));
    for (CONTEXT_INUSE::const_iterator citU = m_InUse.begin(); citU != m_InUse.end(); ++citU)
    {
      (*citU)->debugDump(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_History.size()=" << m_History.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_History={" << std::endl;
  {
    ALock lock(const_cast<ABasePtrQueue *>(&m_History)->useSync());
    for (const ABase *p = m_History.getHead(); p; p = p->getNext())
    {
      dynamic_cast<const ADebugDumpable *>(p)->debugDump(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ErrorHistory.size()=" << m_History.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ErrorHistory={" << std::endl;
  {
    ALock lock(const_cast<ABasePtrQueue *>(&m_ErrorHistory)->useSync());
    for (const ABase *p = m_ErrorHistory.getHead(); p; p = p->getNext())
    {
      dynamic_cast<const ADebugDumpable *>(p)->debugDump(os, indent+2);
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_FreeStoreMaxSize=" << m_FreeStoreMaxSize << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_FreeStore.size=" << m_FreeStore.size() << std::endl;
  
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

  //a_Check if specific context is required
  AString contextId;
  if (request.getUrl().getParameterPairs().get(ASW("contextId",9), contextId))
  {
    //a_Find the specific context
    {
      ALock lock(m_InUseSync);
      for (CONTEXT_INUSE::const_iterator cit = m_InUse.begin(); cit != m_InUse.end(); ++cit)
      {
        if (contextId == AString::fromPointer(*cit))
        {
          adminAddProperty(eBase, ASW("contextDetail",7), *(*cit), AXmlElement::ENC_CDATADIRECT);
          return;
        }
      }
    }
    
    {
      ALock lock(m_ErrorHistory.useSync());
      for (ABase *p = m_ErrorHistory.useTail(); p; p = p->usePrev())
      {
        AOSContext *pContext = dynamic_cast<AOSContext *>(p);
        if (contextId == AString::fromPointer(pContext))
        {
          adminAddProperty(eBase, ASW("contextDetail",7), *pContext, AXmlElement::ENC_CDATADIRECT);
          return;
        }
      }
    }

    {
      ALock lock(m_History.useSync());
      for (ABase *p = m_History.useTail(); p; p = p->usePrev())
      {
        AOSContext *pContext = dynamic_cast<AOSContext *>(p);
        if (contextId == AString::fromPointer(pContext))
        {
          adminAddProperty(eBase, ASW("contextDetail",7), *pContext, AXmlElement::ENC_CDATADIRECT);
          return;
        }
      }
    }

	  //a_Context no longer available
    adminAddError(eBase, ASWNL("AOSContext no longer available"));
  }
  else
  {
    //a_Display context summary
    for (size_t i=0; i < m_Queues.size(); ++i)
    {
      AOSContextQueueInterface *pQueue = m_Queues.at(i);
      if (pQueue)
        adminAddProperty(eBase, AString::fromSize_t(i), pQueue->getClass());
      else
        adminAddProperty(eBase, AString::fromSize_t(i), AConstant::ASTRING_NULL);
    }

    adminAddPropertyWithAction(
      eBase,
      ASW("log_level",9), 
      AString::fromInt(m_DefaultEventLogLevel),
      ASW("Update",6), 
      ASWNL("Maximum event to log 0:Disabled 1:Error, 2:Event, 3:Warning, 4:Info, 5:Debug"),
      ASW("Set",3)
    );

    {
      AString str;
      str.assign('[');
      str.append(AString::fromSize_t(m_History.size()));
      str.append('/');
      str.append(AString::fromSize_t(m_HistoryMaxSize));
      str.append(']');

      AXmlElement& eHistory = eBase.addElement("object").addAttribute("name", "history");
      adminAddPropertyWithAction(eHistory, ASWNL("max_size"), str, ASW("Set",3), ASWNL("Set maximum AOSContext history size (objects not immediately deleted)"), ASW("Set",3));
    }
    
    adminAddPropertyWithAction(
      eBase,
      ASW("clear_history",13), 
      AConstant::ASTRING_EMPTY,
      ASW("Clear",5), 
      ASWNL("Clear context history: 0:all  1:history  2:error history"),
      ASW("Clear",5) 
    );
      
    {
      AString str;
      str.assign('[');
      str.append(AString::fromSize_t(m_ErrorHistory.size()));
      str.append('/');
      str.append(AString::fromSize_t(m_ErrorHistoryMaxSize));
      str.append(']');

      AXmlElement& eErrorHistory = eBase.addElement("object").addAttribute("name", "error_history");
      adminAddPropertyWithAction(eErrorHistory, ASWNL("max_size"), str, ASW("Set",3), ASWNL("Set maximum AOSContext error history size (objects not immediately deleted)"), ASW("Set",3));
    }

    {
      AString str;
      str.assign('[');
      str.append(AString::fromSize_t(m_FreeStore.size()));
      str.append('/');
      str.append(AString::fromSize_t(m_FreeStoreMaxSize));
      str.append(']');

      AXmlElement& eFreestore = eBase.addElement("object").addAttribute("name", "freestore");
      adminAddPropertyWithAction(eFreestore, ASWNL("max_size"), str, ASW("Set",3), ASWNL("Set maximum AOSContext freestore size (objects not immediately deleted)"), ASW("Set",3));
    }

    AXmlElement& eInUse = eBase.addElement("object");
    eInUse.addAttribute("name", "InUse");
    eInUse.addAttribute("size", AString::fromSize_t(m_InUse.size()));
    CONTEXT_INUSE::const_iterator citU = m_InUse.begin();
    if (citU != m_InUse.end())
    {
      ALock lock(m_InUseSync);
      while(citU != m_InUse.end())
      {
        AXmlElement& eProp = adminAddProperty(eInUse, ASW("context",7), (*citU)->useEventVisitor(), AXmlElement::ENC_CDATADIRECT);
        eProp.addAttribute(ASW("errors",6), AString::fromSize_t((*citU)->useEventVisitor().getErrorCount()));
        eProp.addElement(ASW("url",3)).addData((*citU)->useEventVisitor().useName(), AXmlElement::ENC_CDATADIRECT);
        eProp.addElement(ASW("contextId",9)).addData(AString::fromPointer(*citU), AXmlElement::ENC_CDATADIRECT);
        ++citU;
      }
    }

    {
      AXmlElement& eHistory = eBase.addElement("object").addAttribute("name", "History");
      ALock lock(m_History.useSync());
      for (ABase *p = m_History.useTail(); p; p = p->usePrev())
      {
        AOSContext *pContext = dynamic_cast<AOSContext *>(p);
        if (pContext)
        {
          AXmlElement& eProp = adminAddProperty(eHistory, ASW("context",7), pContext->useEventVisitor(), AXmlElement::ENC_CDATADIRECT);
          eProp.addAttribute(ASW("errors",6), AString::fromSize_t(pContext->useEventVisitor().getErrorCount()));
          eProp.addElement(ASW("url",3), pContext->useEventVisitor().useName(), AXmlElement::ENC_CDATADIRECT);
          eProp.addElement(ASW("contextId",3)).addData(AString::fromPointer(pContext), AXmlElement::ENC_CDATADIRECT);
        }
        else
          ATHROW(this, AException::InvalidObject);
      }
    }

    {
      AXmlElement& eErrorHistory = eBase.addElement("object").addAttribute("name", "ErrorHistory");
      ALock lock(m_ErrorHistory.useSync());
      for (ABase *p = m_ErrorHistory.useTail(); p; p = p->usePrev())
      {
        AOSContext *pContext = dynamic_cast<AOSContext *>(p);
        if (pContext)
        {
          AXmlElement& eProp = adminAddProperty(eErrorHistory, ASW("context",7), pContext->useEventVisitor(), AXmlElement::ENC_CDATADIRECT);
          eProp.addAttribute(ASW("errors",6), AString::fromSize_t(pContext->useEventVisitor().getErrorCount()));
          eProp.addElement(ASW("url",3), pContext->useEventVisitor().useName(), AXmlElement::ENC_CDATADIRECT);
          eProp.addElement(ASW("contextId",3)).addData(AString::fromPointer(pContext), AXmlElement::ENC_CDATADIRECT);
        }
        else
          ATHROW(this, AException::InvalidObject);
      }
    }
  }
}

void AOSContextManager::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    if (str.equals(ASWNL("AOSContextManager.log_level")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        int level = str.toInt();
        if (level >= AEventVisitor::EL_NONE && level <= AEventVisitor::EL_DEBUG)
        {
          m_DefaultEventLogLevel = level;
        }
        else
        {
          adminAddError(eBase, ASWNL("Invalid log level, must be (0-5)"));
        }
      }
    }
    else if (str.equals(ASWNL("AOSContextManager.clear_history")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Clear",5), str))
      {
        int level = str.toInt();
        switch(level)
        {
          case 0:
            m_History.clear();
            m_ErrorHistory.clear();
          break;

          case 1:
            m_History.clear();
          break;

          case 2:
            m_ErrorHistory.clear();
          break;

          default:
            adminAddError(eBase, ASWNL("Invalid level for history clearing, must be (0,1,2)"));
        }
      }
    }
    else if (str.equals(ASWNL("AOSContextManager.history.max_size")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        int i = str.toInt();
        if (i >= 0)
        {
          m_HistoryMaxSize = i;
        }
        else
        {
          adminAddError(eBase, ASWNL("Invalid maximum size, must be >=0"));
        }
      }
    }
    else if (str.equals(ASWNL("AOSContextManager.error_history.max_size")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        int i = str.toInt();
        if (i >= 0)
        {
          m_ErrorHistoryMaxSize = i;
        }
        else
        {
          adminAddError(eBase, ASWNL("Invalid maximum size, must be >=0"));
        }
      }
    }
    else if (str.equals(ASWNL("AOSContextManager.freestore.max_size")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        int i = str.toInt();
        if (i >= 0)
        {
          m_FreeStoreMaxSize = i;
        }
        else
        {
          adminAddError(eBase, ASWNL("Invalid maximum size, must be >=0"));
        }
      }
    }
  }
}

AOSContextManager::AOSContextManager(AOSServices& services) :
  m_Services(services),
  m_History(new ASync_CriticalSection()),
  m_ErrorHistory(new ASync_CriticalSection()),
  m_FreeStore(new ASync_CriticalSectionSpinLock())
{
  m_HistoryMaxSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/history-maxsize", 100);
  m_ErrorHistoryMaxSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/error-history-maxsize", 100);
  m_DefaultEventLogLevel = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/log-level", 2);
  AASSERT(this, m_DefaultEventLogLevel >= AEventVisitor::EL_NONE && m_DefaultEventLogLevel <= AEventVisitor::EL_DEBUG);
  m_FreeStoreMaxSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/freestore/max-size", 500);
  size_t freeStoreInitialSize = services.useConfiguration().useConfigRoot().getInt("/config/server/context-manager/freestore/initial-size", 100);
  AASSERT(this, freeStoreInitialSize <= m_FreeStoreMaxSize && freeStoreInitialSize >= 0);

  m_Queues.resize(AOSContextManager::STATE_LAST, NULL);

  adminRegisterObject(m_Services.useAdminRegistry());

  for (int i=0; i<freeStoreInitialSize; ++i)
  {
    m_FreeStore.push(new AOSContext(NULL, m_Services));
  }
}

AOSContextManager::~AOSContextManager()
{
  try
  {
    for (CONTEXT_INUSE::iterator itU = m_InUse.begin(); itU != m_InUse.end(); ++itU)
    {
      delete *itU;
    }

    for (QUEUES::iterator itQ = m_Queues.begin(); itQ != m_Queues.end(); ++itQ)
    {
      delete (*itQ);
    }
    
    m_FreeStore.clear(true);
  }
  catch(...) {}
}

AOSContext *AOSContextManager::allocate(AFile_Socket *pSocket)
{
  AASSERT(this, pSocket);
  AOSContext *p = _newContext(pSocket);
  
  //a_Set logging level
  switch(m_DefaultEventLogLevel)
  {
    case AEventVisitor::EL_DEBUG: p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_DEBUG); break;
    case AEventVisitor::EL_INFO: p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_INFO); break;
    case AEventVisitor::EL_WARN: p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_WARN); break;
    case AEventVisitor::EL_ERROR: p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_ERROR); break;
    case AEventVisitor::EL_NONE: p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_NONE); break;
    default: 
      p->useEventVisitor().setEventThresholdLevel(AEventVisitor::EL_EVENT);

  }

  ARope rope("AOSContextManager::allocate[",28);
  rope.append(AString::fromPointer(p));
  rope.append(", pFile=",8);
  rope.append(AString::fromPointer(pSocket));
  rope.append("] new create",12);
  p->useEventVisitor().startEvent(rope);

  {
    ALock lock(m_InUseSync);
    AASSERT(this, p && m_InUse.end() == m_InUse.find(p));
    m_InUse.insert(p);
  }

  AASSERT(this, &p->useSocket());
  return p;
}

void AOSContextManager::deallocate(AOSContext *p)
{
  if (p)
  {
    {
      ALock lock(m_InUseSync);
      CONTEXT_INUSE::iterator it = m_InUse.find(p);
      AASSERT(this, p && m_InUse.end() != it);
    
      m_InUse.erase(it);
    }
  }
  else
    return;

  //a_Log
  ARope rope("AOSContextManager::deallocate[",30);
  rope.append(AString::fromPointer(p));
  rope.append(']');
  p->useEventVisitor().startEvent(rope);
  
  //a_Write to log
  if (p->useEventVisitor().getErrorCount() > 0)
  {
    m_Services.useLog().add(p->useEventVisitor(), ALog::CRITICAL_ERROR);
  }

  if (p->useEventVisitor().getErrorCount() > 0)
  {
    //a_Add to error history
    if (m_ErrorHistoryMaxSize > 0)
    {
      p->finalize();
      m_ErrorHistory.push(p);
    }
    else
      _deleteContext(&p);

    while (m_ErrorHistory.size() > m_ErrorHistoryMaxSize)
    {
      //a_Remove last
      AOSContext *pC = (AOSContext *)m_ErrorHistory.pop();
      _deleteContext(&pC);
    }
  }
  else
  {
    //a_Add to history
    if (m_HistoryMaxSize > 0)
    {
      p->finalize();
      m_History.push(p);
    }
    else
    {
      _deleteContext(&p);
    }

    while (m_History.size() > m_HistoryMaxSize)
    {
      //a_Remove last
      AOSContext *pC = (AOSContext *)m_History.pop();
      if (pC)
        _deleteContext(&pC);
    }
  }
}

AOSContext *AOSContextManager::_newContext(AFile_Socket *pSocket)
{
  AOSContext *pContext = (AOSContext *)m_FreeStore.pop();
  
  if (!pContext)
    return new AOSContext(pSocket, m_Services);
  else
  {
    pContext->reset(pSocket);
    return pContext;
  }
}

void AOSContextManager::_deleteContext(AOSContext **p)
{
  AASSERT(this, NULL != *p);
  if (m_FreeStore.size() >= m_FreeStoreMaxSize)
    delete(*p);
  else
  {
    (*p)->finalize(true);
    m_FreeStore.push(*p);
  }

  p = NULL;
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
