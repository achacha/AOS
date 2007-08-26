#include "pchUnitTests.hpp"
#include "ASocketListener.hpp"
#include "AFile_Socket.hpp"
#include "AThread.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"

ASocketLibrary g_SockerLibrary;

#define UNIT_TEST_PORT 19997
#define UNIT_TEST_HOST "127.0.0.1"
#define UNIT_TEST_REQUEST "X-UnitTest-Id"
#define UNIT_TEST_RESPONSE "X-UnitTest-Response"
u4 threadprocServer(AThread& thread)
{
  int& iRet = *(int *)thread.getParameter();
  
  ASocketListener listener(UNIT_TEST_PORT, UNIT_TEST_HOST);
  try
  {
    AHTTPRequestHeader requestHeader;
    AHTTPResponseHeader responseHeader;
    listener.open();
    thread.setRunning(true);
    while(thread.isRun())
    {
      if (listener.isAcceptWaiting())
      {
        AString testId;
        AFile_Socket connection(listener, true);
        connection.open();

        requestHeader.clear();
        responseHeader.clear();

        requestHeader.fromAFile(connection);

        if (requestHeader.find(UNIT_TEST_REQUEST, testId))
        {
          responseHeader.setStatusCode(AHTTPResponseHeader::SC_200_Ok);
          if (testId.equals("1"))
          {
            responseHeader.setPair(UNIT_TEST_RESPONSE, "echo");
          }
          else
          {
            responseHeader.setStatusCode(AHTTPResponseHeader::SC_501_Not_Implemented);
            std::cerr << "ut_AFile_Socket_General: testId specified does not exist: " << testId << std::endl;
          }
        }
        else
        {
          responseHeader.setStatusCode(AHTTPResponseHeader::SC_500_Internal_Server_Error);
        }
        responseHeader.toAFile(connection);
        connection.close();
        break;
      }
      else
        AThread::sleep(500);
    }
  }
  catch(AException& ex)
  {
    std::cerr << "ut_AFile_Socket_General::threadprocServer: " << ex.what() << std::endl;
    ++iRet;
  }
  catch(...)
  {
    std::cerr << "Unknown exception in ut_AFile_Socket_General::threadprocServer" << std::endl;
    ++iRet;
  }
  listener.close();
  thread.setRunning(false);

  return 0;
}

void testConversionIp4(int& iRet)
{
  AString strIp1("1.2.3.4");
  u4 u4Ip = ASocketLibrary::convertStringToIp4(strIp1);

  AString strIp2;
  ASocketLibrary::convertIp4ToString(u4Ip, strIp2);

  ASSERT_UNIT_TEST(strIp1.equals(strIp2), "ASocketLibrary IP4 convert", "0", iRet);
}

int ut_AFile_Socket_General()
{
  std::cerr << "ut_AFile_Socket_General" << std::endl;

  int iRet = 0x0;

  testConversionIp4(iRet);

  AThread server(threadprocServer, true, (void *)&iRet);
  int timeout = 10;
  while (!server.isRunning() && timeout > 0)
  {
    AThread::sleep(300);
    --timeout;
  }
  if (!timeout)
  {
    ASSERT_UNIT_TEST(timeout, "AThread for server did not start", "", iRet);
    return iRet;
  }

  AFile_Socket client(UNIT_TEST_HOST, UNIT_TEST_PORT, true);
  client.open();

  AHTTPRequestHeader request;
  AHTTPResponseHeader response;
  request.setPair(AHTTPHeader::HT_REQ_Host, UNIT_TEST_HOST);
  request.setPair(UNIT_TEST_REQUEST, "1");
  request.toAFile(client);
  response.fromAFile(client);

  AString str;
//  request.debugDump();
//  response.debugDump();
  ASSERT_UNIT_TEST(response.find(UNIT_TEST_RESPONSE, str), "Response pair not found", "", iRet);
  ASSERT_UNIT_TEST(str.equals("echo"), "Response pair contains invalid value", "", iRet);

  server.setRun(false);
  timeout = 10;
  while (server.isRunning() && timeout > 0)
  {
    AThread::sleep(100);
    --timeout;
  }
  if (!timeout)
  {
    ASSERT_UNIT_TEST(timeout, "AThread for server did not stop, terminating", "", iRet);
    server.terminate(-1);
  }

  return iRet;
}
