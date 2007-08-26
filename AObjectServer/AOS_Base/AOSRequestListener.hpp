#ifndef INCLUDED__AOSRequestListener_HPP__
#define INCLUDED__AOSRequestListener_HPP__

#include "apiAOS_Base.hpp"
#include "AThread.hpp"
#include "ASync_CriticalSection.hpp"
#include "AOSContext.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;
class AOSContextQueueInterface;

class AOS_BASE_API AOSRequestListener : public AOSAdminInterface
{
public:
  /*!
  Create HTTP and HTTPS listeners
  pFirstQueue - first queue set to receive AOSRequest* once accepted (not OWNED by this object)
  **/
  AOSRequestListener(
    AOSServices&, 
    AOSContextQueueInterface *pFirstQueue
  );
  ~AOSRequestListener();
  
  /*!
  Start listener threads
  **/
  void startListening();
  void stopListening();
  bool isRunning() const;

  /*!
  AOSAdminInterface
  **/
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //a_Listener threads
  AThread mthread_Listener;
  AThread mthread_SecureListener;
  
  struct LISTEN_DATA
  {
    LISTEN_DATA() : port(80), host(ASocketLibrary::ANY_ADDRESS), pListener(NULL) {}

    int port;
    AString host;
    
    //a_Used for HTTPS connections, empty otherwise
    AString cert;
    AString pkey;

    //a_Owner
    AOSRequestListener *pListener;
  };

  //a_Listeners
  
  
  //a_Queues
  ASync_CriticalSection m_QueueAdd;
  ASync_CriticalSection m_QueueRemove;

  //a_First queue
  AOSContextQueueInterface *mp_FirstQueue;
  
  //a_External references
  AOSServices& m_Services;
  
  static u4 threadprocListener(AThread&);
  static u4 threadprocSecureListener(AThread&);
};


#endif // INCLUDED__AOSRequestListener_HPP__
