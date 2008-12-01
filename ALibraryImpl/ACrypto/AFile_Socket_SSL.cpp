/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchACrypto.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketException.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ASocketListener_SSL.hpp"

#define CHK_NULL(x) if ((x)==NULL) assert(0)

struct SSLData
{
  SSLData() : ctx(NULL), ssl(NULL), server_cert(NULL), meth(NULL)
  {
  }
  
  ~SSLData()
  { 
    if (ssl) 
      SSL_free(ssl);
  }

  SSL_CTX*    ctx;
  SSL*        ssl;
  X509*       server_cert;
  SSL_METHOD* meth;
};

void AFile_Socket_SSL::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  AFile_Socket::debugDump(os,indent+1);
  ADebugDumpable::indent(os, indent+1) << "mp_SSLData->ctx=0x" << std::hex << (void *)((SSLData *)mp_SSLData)->ctx << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_SSLData->ssl=0x" << std::hex << (void *)((SSLData *)mp_SSLData)->ssl << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_SSLData->server_cert=0x" << std::hex << (void *)((SSLData *)mp_SSLData)->server_cert << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_SSLData->meth=0x" << std::hex << (void *)((SSLData *)mp_SSLData)->meth << std::dec << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFile_Socket_SSL::AFile_Socket_SSL(const AString& address, int portNum) :
  AFile_Socket(address, portNum, true),
  mp_SSLData(new SSLData())
{
}

AFile_Socket_SSL::AFile_Socket_SSL(ASocketListener_SSL& listener) :
  AFile_Socket(listener, true),
  mp_SSLData(new SSLData())
{
}

AFile_Socket_SSL::~AFile_Socket_SSL()
{
  try
  {
    close();
    delete mp_SSLData;
  }
  catch(...) {}
}

void AFile_Socket_SSL::open()
{
  AASSERT(this, mbool_Blocking);  //a_Must be blocking for SSL for now
  if (mp_Listener)
  {
    _initSSL(false);
    m_SocketInfo = ((ASocketListener_SSL *)mp_Listener)->accept(mp_SSLData);
  }
  else
  {
    _initSSL(true);
    AFile_Socket::open();
    _connectSSL();
  }

  m_EOF = false;
}

void AFile_Socket_SSL::close()
{
  AFile_Socket::close();
  _deinitSSL();
}

size_t AFile_Socket_SSL::_write(const void *buf, size_t size)
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;

  if (!pData || size <= 0x0)
	  return AConstant::npos;

  size_t bytesToWrite = size;
  size_t totalBytesWritten = 0;

  //a_It may take multiple calls to send the entire buffer
  while (bytesToWrite)
  {
    int bytesWritten = SSL_write(pData->ssl, (const char *) buf + totalBytesWritten, bytesToWrite);
    if (-1 == bytesWritten)
      ATHROW(this, AException::OperationFailed);

    if (bytesWritten == 0)
      ATHROW(this, ASocketException::WriteOperationFailed);
      
    if (bytesWritten == SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);

    bytesToWrite -= bytesWritten;      //reduce bytes to write
    totalBytesWritten += bytesWritten; //increase offset for buffer
  }

  return totalBytesWritten;
}

size_t AFile_Socket_SSL::readBlocking(void *buf, size_t size)
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;

  if (!pData || size == 0x0)
    return 0;
  
  size_t bytesToRead = size;
  size_t totalBytesRead = 0;

  //a_It may take multiple calls to send the entire buffer
  while (bytesToRead)
  {
    int bytesReceived = SSL_read(pData->ssl, (char *) buf + totalBytesRead, bytesToRead);
    if (bytesReceived == 0)
      return totalBytesRead; //a_Socket closed

    if (bytesReceived == SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);

    bytesToRead -= bytesReceived; //reduce bytes to read
    totalBytesRead += bytesReceived; //increase offset for buffer
  }

  return totalBytesRead;
}

size_t AFile_Socket_SSL::_read(void *buf, size_t size)
{
  if (m_EOF)
    return 0;

  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;

  if (!pData)
    return AConstant::npos;

  if (mbool_Blocking)
    return readBlocking(buf, size);
  
  //a_Cannot read zero bytes, return of 0 means EOF
  if ( size <= 0 )
    ATHROW(this, ASocketException::InvalidParameter);
  
  size_t bytesReceived = SSL_read(pData->ssl, buf, size);
  if (bytesReceived == SOCKET_ERROR)
  {
    int err = ::WSAGetLastError();
    if (err == WSAEWOULDBLOCK)
      return 0;

    ATHROW_LAST_SOCKET_ERROR_KNOWN(this, err);
  }

  //a_Blocking mode received less than requested implies EOF
  if (mbool_Blocking && bytesReceived < size)
    m_EOF = true;

  return bytesReceived;
}

void AFile_Socket_SSL::_initSSL(bool isOutboundSocket)
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;
  if (!pData)
    ATHROW(this, AException::InvalidObject);

  pData->meth = SSLv23_client_method();
  pData->ctx = SSL_CTX_new(pData->meth);
  AASSERT(this, pData->ctx);

  //a_Outbound sockets need SSL class, accepted ones will use context of the server to create it
  if (isOutboundSocket)
  {
    AASSERT(this, !pData->ssl);
    if (NULL == (pData->ssl = SSL_new(pData->ctx)))
      ATHROW_EX(this, ASocketException::APIFailure, ASWNL("Unable to allocate SSL object"));

  }
}
void AFile_Socket_SSL::_connectSSL()
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;
  if (!pData)
    ATHROW(this, AException::InvalidObject);

  /* ----------------------------------------------- */
  /* Now we have TCP conncetion. Start SSL negotiation. */
  SSL_set_connect_state(pData->ssl);
  
  BIO *bio = BIO_new_socket(m_SocketInfo.m_handle, BIO_NOCLOSE);
  if (!bio)
  {
    ATHROW_EX(this, ASocketException::APIFailure, ASWNL("Unable to bind socket descriptor to SSL"));
  }
  SSL_set_bio(pData->ssl, bio, bio);

  int ret;
  if (0 >= (ret = SSL_connect(pData->ssl)))
  {
    throwSSLError(SSL_get_error(pData->ssl, ret));
  }
    
  /* Following two steps are optional and not required for data exchange to be successful. */
  /* Get the cipher - opt */
//  std::cout << "SSL connection using " << SSL_get_cipher(pData->ssl) << std::endl;
  
  /* Get server's certificate (note: beware of dynamic allocation) - opt */
//  pData->server_cert = SSL_get_peer_certificate(pData->ssl);
//  CHK_NULL(pData->server_cert);
//  printf ("Server certificate:\n");
  
//  char *str = X509_NAME_oneline(X509_get_subject_name(pData->server_cert),0,0);
//  CHK_NULL(str);
//  printf ("\t subject: %s\n", str);
//  OPENSSL_free (str);

//  str = X509_NAME_oneline(X509_get_issuer_name(pData->server_cert),0,0);
//  CHK_NULL(str);
//  printf ("\t issuer: %s\n", str);
//  OPENSSL_free(str);

  /* We could do all sorts of certificate verification stuff here before deallocating the certificate. */
//  X509_free(pData->server_cert);
//  pData->server_cert=NULL;
}

void AFile_Socket_SSL::throwSSLError(int sslErrorCode)
{
  switch(sslErrorCode)
  {
    case SSL_ERROR_ZERO_RETURN:
//      ATHROW_EX(this, AException::APIFailure, 
//"The TLS/SSL connection has been closed.  If the protocol version \
//is SSL 3.0 or TLS 1.0, this result code is returned only if a closure \
//alert has occurred in the protocol, i.e. if the connection \
//has been closed cleanly. Note that in this case \
//SSL_ERROR_ZERO_RETURN does not necessarily indicate that the \
//underlying transport has been closed.");
      //a_Do nothing in this case
      break;

    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      ATHROW_EX(this, AException::APIFailure, 
ASWNL("The operation did not complete; the same TLS/SSL I/O function \
should be called again later.  If, by then, the underlying BIO has \
data available for reading (if the result code is \
SSL_ERROR_WANT_READ) or allows writing data \
(SSL_ERROR_WANT_WRITE), then some TLS/SSL protocol progress will \
take place, i.e. at least part of an TLS/SSL record will be read \
or written.  Note that the retry may again lead to a \
SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE condition.  There is \
no fixed upper limit for the number of iterations that may be necessary \
until progress becomes visible at application protocol level.\
\r\n\
For socket BIOs (e.g. when SSL_set_fd() was used), select() or \
poll() on the underlying socket can be used to find out when the \
TLS/SSL I/O function should be retried.\
\r\n\
Caveat: Any TLS/SSL I/O function can lead to either of \
SSL_ERROR_WANT_READ and SSL_ERROR_WANT_WRITE.  In particular, \
SSL_read() or SSL_peek() may want to write data and SSL_write() \
may want to read data.  This is mainly because TLS/SSL handshakes \
may occur at any time during the protocol (initiated by either the \
client or the server); SSL_read(), SSL_peek(), and SSL_write() \
will handle any pending handshakes."));
      break;

    case SSL_ERROR_WANT_CONNECT:
    case SSL_ERROR_WANT_ACCEPT:
      ATHROW_EX(this, AException::APIFailure, 
ASWNL("The operation did not complete; the same TLS/SSL I/O function \
should be called again later. The underlying BIO was not connected \
yet to the peer and the call would block in connect()/accept(). \
The SSL function should be called again when the connection is \
established. These messages can only appear with a BIO_s_connect() \
or BIO_s_accept() BIO, respectively.  In order to find out, when \
the connection has been successfully established, on many platforms \
select() or poll() for writing on the socket file descriptor can be used."));
      break;

    case SSL_ERROR_WANT_X509_LOOKUP:
      ATHROW_EX(this, AException::APIFailure, 
ASWNL("The operation did not complete because an application callback set \
by SSL_CTX_set_client_cert_cb() has asked to be called again.  The \
TLS/SSL I/O function should be called again later.  Details depend \
on the application."));
      break;

    case SSL_ERROR_SYSCALL:
    {
      u4 error = ::GetLastError();
      ATHROW_LAST_SOCKET_ERROR_KNOWN(this, error);
    }
    break;

    case SSL_ERROR_SSL:
    {
      AString strErrors(4096, 1024);
      int errorCode;
      while (errorCode = ERR_get_error())
      {
        char errorString[1024];
        ERR_error_string(errorCode, errorString);
        strErrors.append(errorString);
        strErrors.append(AConstant::ASTRING_EOL);
      }

      ATHROW_EX(this, AException::APIFailure, strErrors);
    }
    break;

    default:
    case SSL_ERROR_NONE:
      //a_Do nothing
    break;
  }
}

void AFile_Socket_SSL::_deinitSSL()
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;
  if (!pData)
    return;

  SSL_free(pData->ssl);
  SSL_CTX_free(pData->ctx);
  
  memset(pData, 0, sizeof(SSLData));
}

size_t AFile_Socket_SSL::readBlockIntoLookahead()
{
  if (m_EOF)
    return 0;
  
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;
  if (!pData)
    return AConstant::npos;

  char p[AFile::DefaultReadBlock];
  size_t bytesAvailable = AFile::DefaultReadBlock;
  size_t bytesRead = SSL_read(pData->ssl, p, bytesAvailable);
  if (AConstant::npos != bytesRead)
  {
    m_LookaheadBuffer.pushBack(p, bytesRead);
  }

  //a_In blocking mode, if we read less than requested, we are at EOF
  if (mbool_Blocking && bytesRead < bytesAvailable)
    m_EOF = true;

  return bytesRead;
}

size_t AFile_Socket_SSL::_availableInputWaiting() const
{
  AASSERT(this, mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;
  if (!pData)
    return AConstant::npos;

  u4 bytes = SSL_pending(pData->ssl);
  return bytes;
}
