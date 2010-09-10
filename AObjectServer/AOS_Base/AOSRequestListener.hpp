/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSRequestListener_HPP__
#define INCLUDED__AOSRequestListener_HPP__

#include "apiAOS_Base.hpp"
#include "AFilename.hpp"
#include "AThread.hpp"
#include "ASync_CriticalSection.hpp"
#include "AOSContext.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSContextManager.hpp"

class AOSServices;

class AOS_BASE_API AOSRequestListener : public AOSAdminInterface
{
public:
  static const AString CLASS;

public:
  /*!
  Create HTTP and HTTPS listeners
  firstQueue - first queue to receive AOSContext* once accepted (not OWNED by this object)
  */
  AOSRequestListener(
    AOSServices&, 
    AOSContextManager::ContextQueueState firstQueue
  );
  ~AOSRequestListener();
  
  /*!
  Start listening on the socket but not processing requests
  */
  void startListening();
  
  /*!
  Start accepting and processing requests
  */
  void startAccepting();
  
  /*!
  Stop processing and listening
  */
  void stop();
  
  /*!
  Check if processing requests
  */
  bool isRunning() const;

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //a_Listener threads
  AThread mthread_Listener;
  AThread mthread_SecureListener;
  
  class LISTEN_DATA : public ABase
  {
  public:
    LISTEN_DATA(AOSServices& services, const AString& configpath);
    
    int getPort() const;
    const AString& getHost() const;
    const AFilename& getCertificateFilename() const;
    const AFilename& getPrivateKeyFilename() const;

    // If socket should start processing
    void setRun(bool b);
    bool isRun() const;

  private:
    bool m_isRun;

    AFilename m_cert;
    AFilename m_pkey;
    int m_port;
    AString m_host;

    LISTEN_DATA() {}
  };
  
  // Listen data for each socket
  LISTEN_DATA *mp_HttpData;
  LISTEN_DATA *mp_HttpsData;

  //a_Queues
  ASync_CriticalSection m_QueueAdd;
  ASync_CriticalSection m_QueueRemove;

  //a_First queue
  AOSContextManager::ContextQueueState m_FirstQueue;
  
  //a_External references
  AOSServices& m_Services;
  
  static u4 threadprocListener(AThread&);
  static u4 threadprocSecureListener(AThread&);
};


#endif // INCLUDED__AOSRequestListener_HPP__
