#ifndef INCLUDED__AOSContextManager_HPP__
#define INCLUDED__AOSContextManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSContext.hpp"
#include "ACriticalSection.hpp"

class AOSServices;

class AOS_BASE_API AOSContextManager : public AOSAdminInterface
{
public:
  AOSContextManager(AOSServices&);
  virtual ~AOSContextManager();
  
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
  //a_Contexts currently in use
  typedef std::map<AOSContext *, int> CONTEXT_INUSE;
  CONTEXT_INUSE m_InUse;
  ACriticalSection m_InUseSync;
  
  //a_Context history, after they scroll off here they go into the freestore
  typedef std::list<AOSContext *> CONTEXT_HISTORY;
  CONTEXT_HISTORY m_History;
  ACriticalSection m_HistorySync;
  size_t m_HistoryMaxSize;

  //a_Contexts available for reuse
  typedef std::deque<AOSContext *> CONTEXT_FREESTORE;
  CONTEXT_FREESTORE m_Freestore;
  ACriticalSection m_FreestoreSync;
  size_t m_FreestoreMaxSize;

  //a_Reference to the services
  AOSServices& m_Services;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSContextManager_HPP__
