#ifndef INCLUDED__AOSAdmin_HPP__
#define INCLUDED__AOSAdmin_HPP__

#include "apiAOS_Base.hpp"
#include "AThread.hpp"
#include "AMutex.hpp"

class AFile_Socket;
class AHTTPRequestHeader;
class AXmlDocument;
class AOSServices;

/*!
Administration
*/
class AOS_BASE_API AOSAdmin
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

protected:
  static u4 threadprocAdminListener(AThread& thread);

private:
  AThread mthread_AdminListener;
  AMutex m_ClientSynch;

  //a_Process a request
  void _processRequest(AFile_Socket&, AHTTPRequestHeader&);
  void _processAdmin(AFile_Socket&, AHTTPRequestHeader&);
  void _processAdminCommand(const AString&, AHTTPRequestHeader&, AHTTPResponseHeader&, AOutputBuffer&);

  //a_Output helpers
  void _prepareXmlDocument(const AString&, AHTTPRequestHeader&, AXmlDocument&);

  AOSServices& m_Services;
};

#endif //INCLUDED__AOSAdmin_HPP__
