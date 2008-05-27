/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchACrypto.hpp"
#include "ASocketListener_SSL.hpp"
#include "ASocketException.hpp"
#include "AFile_Socket.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "AFileSystem.hpp"

/* Make these what you want for cert & key files */
#define CERTF "aos_cert.pem"
#define KEYF  "aos_key.pem"

#define CHK_NULL(x) if ((x)==NULL) assert(0)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); assert(0); }

struct SSLData
{
  SSLData() : ctx(NULL), ssl(NULL), client_cert(NULL), meth(NULL) {}
  SSL_CTX*    ctx;
  SSL*        ssl;
  X509*       client_cert;
  SSL_METHOD* meth;
};

void ASocketListener_SSL::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ASocketListener_SSL @ " << std::hex << this << std::dec << ") {" << std::endl;
  ASocketListener::debugDump(os,indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ASocketListener_SSL::ASocketListener_SSL(
  int port,
  const AString& certFilename, 
  const AString& keyFilename, 
  const AString& ip,              // = AConstant::ASTRING_EMPTY
  int backlog,                    // = 5 
  bool allowLocalReuse            // = false
) :
  ASocketListener(port, ip, backlog, allowLocalReuse),
  mp_SSLData(new SSLData()),
  m_certFilename(certFilename),
  m_keyFilename(keyFilename)
{
}

ASocketListener_SSL::~ASocketListener_SSL()
{
  try
  {
    close();
    delete mp_SSLData;
  }
  catch(...) {}
}

void ASocketListener_SSL::open()
{
  ASocketListener::open();
  _initSSL();
}

void ASocketListener_SSL::_initSSL()
{
  AASSERT(this, mp_SSLData);
  SSLData *pServerData = (SSLData *)mp_SSLData;

  pServerData->meth = SSLv23_server_method();
  pServerData->ctx = SSL_CTX_new(pServerData->meth);
  if (!pServerData->ctx)
  {
    AString strError(1024, 256);
    ERR_error_string(errno, strError.startUsingCharPtr());
    strError.stopUsingCharPtr();
    strError.append(" (possibly ASocketLibrary_SSL global object missing)");
    ATHROW_EX(this, AException::APIFailure, strError);
  }
  
  int retCode;
  if (!AFileSystem::exists(AFilename(m_certFilename, false)))
    ATHROW_EX(this, AException::DoesNotExist, m_certFilename);
  
  if ((retCode = SSL_CTX_use_certificate_file(pServerData->ctx, m_certFilename.c_str(), SSL_FILETYPE_PEM)) <= 0)
  {
    AString strError(1024, 256);
    ERR_error_string(retCode, strError.startUsingCharPtr());
    strError.stopUsingCharPtr();
    ATHROW_EX(this, AException::APIFailure, strError);
  }

  if (!AFileSystem::exists(AFilename(m_keyFilename, false)))
    ATHROW_EX(this, AException::DoesNotExist, m_keyFilename);

  if ((retCode = SSL_CTX_use_PrivateKey_file(pServerData->ctx, m_keyFilename.c_str(), SSL_FILETYPE_PEM)) <= 0)
  {
    AString strError(1024, 256);
    ERR_error_string(retCode, strError.startUsingCharPtr());
    strError.stopUsingCharPtr();
    ATHROW_EX(this, AException::APIFailure, strError);
  }

  if (!(retCode = SSL_CTX_check_private_key(pServerData->ctx)))
  {
    AString strError(1024, 256);
    ERR_error_string(retCode, strError.startUsingCharPtr());
    strError.stopUsingCharPtr();
    ATHROW_EX(this, AException::APIFailure, strError);
  }

  pServerData->ssl = SSL_new(pServerData->ctx);
  AASSERT(this, pServerData->ssl);
}

ASocketLibrary::SocketInfo ASocketListener_SSL::accept(void *pSSLData)
{
  AASSERT(this, pSSLData);
  AASSERT(this, mp_SSLData);
  SSLData *pClientData = (SSLData *)pSSLData;
  SSLData *pServerData = (SSLData *)mp_SSLData;

  ASocketLibrary::SocketInfo info = ASocketListener::accept();
  
  BIO *sbio=BIO_new_socket(info.m_handle, BIO_NOCLOSE);
  pClientData->ssl = SSL_new(pServerData->ctx);
  SSL_set_bio(pClientData->ssl,sbio,sbio);

  int retCode;
  if ((retCode = SSL_accept(pClientData->ssl)) <=0)
  {
    int errorCode = SSL_get_error(pClientData->ssl, retCode);
    throwSSLError(errorCode);
  }

  _initClientSSL(pClientData);

  return info;
}

void ASocketListener_SSL::_initClientSSL(void *pSSLData)
{
  AASSERT(this, pSSLData);
  SSLData *pData = (SSLData *)pSSLData;
  
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
  pData->client_cert = SSL_get_peer_certificate(pData->ssl);
  if (pData->client_cert != NULL) {
    std::cout << "Client certificate:\r\n";
    
    char *str = X509_NAME_oneline(X509_get_subject_name(pData->client_cert), 0, 0);
    CHK_NULL(str);
    std::cout << "\t subject: \r\n" << str;
    OPENSSL_free(str);
    
    str = X509_NAME_oneline(X509_get_issuer_name(pData->client_cert), 0, 0);
    CHK_NULL(str);
    std::cout << "\t issuer: \r\n" << str << std::endl;
    OPENSSL_free(str);
    
    /* We could do all sorts of certificate verification stuff here before deallocating the certificate. */
    X509_free(pData->client_cert);
    pData->client_cert = NULL;
  }
//  else
//    printf ("Client does not have certificate.\n");
}

void ASocketListener_SSL::close()
{
  assert(mp_SSLData);
  SSLData *pData = (SSLData *)mp_SSLData;

//  if (pData->bio)
 //   BIO_free(pData->bio);

  SSL_free(pData->ssl);
  SSL_CTX_free(pData->ctx);

  memset(pData, 0, sizeof(SSLData));
}

void ASocketListener_SSL::throwSSLError(int sslErrorCode)
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
