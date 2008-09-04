/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextManager_HPP__
#define INCLUDED__AOSContextManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSContext.hpp"
#include "ASync_CriticalSection.hpp"
#include "ABasePtrQueue.hpp"

class AOSServices;
class AOSContextQueueInterface;

class AOS_BASE_API AOSContextManager : public AOSAdminInterface
{
public:
  enum ContextQueueState
  {
    STATE_TERMINATE = 0,
    STATE_PRE_EXECUTE = 1,
    STATE_EXECUTE = 2,
    STATE_IS_AVAILABLE = 3,
    STATE_ERROR = 4,
    STATE_LAST = 5   // This MUST be the last state
  };

public:
  /*!
  ctor
  */
  AOSContextManager(AOSServices&);
  
  /*!
  dtor
  */
  virtual ~AOSContextManager();
  
  /*!
  Advance context to next queue state

  @param state
  @param context pointer will be changed to NULL if state is advanced
  */
  void changeQueueState(AOSContextManager::ContextQueueState state, AOSContext **ppContext);

  /*!
  Set context queue for the given state

  @param state
  @param pointer to queue, owned and deleted by this class
  */
  void setQueueForState(AOSContextManager::ContextQueueState state, AOSContextQueueInterface *pQueue);

  /*!
  Allocate and deallocate AOSContext
  */
  AOSContext *allocate(AFile_Socket *);
  void deallocate(AOSContext *);

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Container for state to queue
  typedef std::vector<AOSContextQueueInterface *> QUEUES;
  QUEUES m_Queues;
  
  //! Contexts currently in use
  typedef std::set<AOSContext *> CONTEXT_INUSE;
  CONTEXT_INUSE m_InUse;
  ASync_CriticalSection m_InUseSync;
  
  //! Context history, after they scroll off here they are deleted
  ABasePtrQueue m_History;
  size_t m_HistoryMaxSize;

  //! Error context history, after they scroll off here they are deleted
  ABasePtrQueue m_ErrorHistory;
  size_t m_ErrorHistoryMaxSize;

  //! Logging level for all event visitors in the context
  int m_DefaultEventLogLevel;

  //! Reference to the services
  AOSServices& m_Services;

  //! Internal allocate
  AOSContext *_newContext(AFile_Socket *pSocket);

  //! Internal deallocate
  void _deleteContext(AOSContext **);

  //! Maximum size of the freestore of AOSContext*s
  size_t m_FreeStoreMaxSize;

  //! Store allocated and unused AOSContext objects
  ABasePtrQueue m_FreeStore;
};

#endif //INCLUDED__AOSContextManager_HPP__
