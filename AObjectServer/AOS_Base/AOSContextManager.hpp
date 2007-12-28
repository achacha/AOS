#ifndef INCLUDED__AOSContextManager_HPP__
#define INCLUDED__AOSContextManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSContext.hpp"
#include "ASync_CriticalSection.hpp"

class AOSServices;
class AOSContextQueueInterface;

class AOS_BASE_API AOSContextManager : public AOSAdminInterface
{
public:
  enum ContextQueueState
  {
    STATE_PRE_EXECUTE = 0,
    STATE_EXECUTE = 1,
    STATE_IS_AVAILABLE = 2,
    STATE_ERROR = 3,
    STATE_TERMINATE = 4   // This MUST be the last state
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
  virtual void addAdminXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //! Container for state to queue
  typedef std::vector<AOSContextQueueInterface *> QUEUES;
  QUEUES m_Queues;
  
  //! Contexts currently in use
  typedef std::map<AOSContext *, int> CONTEXT_INUSE;
  CONTEXT_INUSE m_InUse;
  ASync_CriticalSection m_InUseSync;
  
  //! Context history, after they scroll off here they go into the freestore
  typedef std::list<AOSContext *> CONTEXT_HISTORY;
  CONTEXT_HISTORY m_History;
  ASync_CriticalSection m_HistorySync;
  size_t m_HistoryMaxSize;

  //! Contexts available for reuse
  typedef std::deque<AOSContext *> CONTEXT_FREESTORE;
  CONTEXT_FREESTORE m_Freestore;
  ASync_CriticalSection m_FreestoreSync;
  size_t m_FreestoreMaxSize;

  //! Reference to the services
  AOSServices& m_Services;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSContextManager_HPP__
