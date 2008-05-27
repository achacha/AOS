/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSAdmin_HPP__
#define INCLUDED__AOSAdmin_HPP__

#include "apiAOS_Base.hpp"
#include "ABase.hpp"
#include "AThread.hpp"
#include "ASync_Mutex.hpp"

class AFile_Socket;
class AHTTPRequestHeader;
class AXmlDocument;
class AOSServices;

/*!
Administration
*/
class AOS_BASE_API AOSAdmin : public ABase
{
public:
  AOSAdmin(AOSServices&);

  /*!
  Socket listener to administration
  */
  void startAdminListener();
  void stopAdminListener();

  /*!
  isRun - true if admin should be running
  isRunning - the actual state of the admin thread
  */
  bool isRun();
  bool isRunning();

  /*!
  Shutdown has been requested, main loop can exit when admin is done running
  */
  bool isShutdownRequested() const;

protected:
  static u4 threadprocAdminListener(AThread& thread);

private:
  AThread mthread_AdminListener;
  ASync_Mutex m_ClientSynch;

  //a_Process a request
  void _processRequest(AFile_Socket&, AHTTPRequestHeader&);
  void _processAdmin(AFile_Socket&, AHTTPRequestHeader&);
  void _processAdminCommand(const AString&, AHTTPRequestHeader&, AHTTPResponseHeader&, AOutputBuffer&);

  //a_Output helpers
  void _prepareXmlDocument(const AString&, AHTTPRequestHeader&, AXmlDocument&);

  AOSServices& m_Services;

  //a_Shutdown requested
  bool m_IsShutdownRequested;
};

#endif //INCLUDED__AOSAdmin_HPP__
