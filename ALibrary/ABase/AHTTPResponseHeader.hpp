#ifndef INCLUDED__AHTTPResponseHeader_HPP__
#define INCLUDED__AHTTPResponseHeader_HPP__

#include "apiABase.hpp"
#include "AHTTPHeader.hpp"
#include "ACookies.hpp"

class ABASE_API AHTTPResponseHeader : public AHTTPHeader
{
public:
  //a_1xx - informational
  //a_2xx - sucess
  //a_3xx - redirection
  //a_4xx - client error
  //a_5xx - server error
  enum STATUS_CODES
  {
    SC_100_Continue                          = 100,
    SC_101_Switching_Protocols               = 101,
    SC_200_Ok                                = 200,
    SC_201_Created                           = 201,
    SC_202_Accepted                          = 202,
    SC_203_Non_Authoritative                 = 203,
    SC_204_No_Content                        = 204,
    SC_205_Reset_Content                     = 205,
    SC_206_Partial_Content                   = 206,
    SC_300_Multiple_Choices                  = 300,
    SC_301_Moved_Permanently                 = 301,
    SC_302_Moved_Temporarily                 = 302,
    SC_303_See_Other                         = 303,
    SC_304_Not_Modified                      = 304,
    SC_305_Use_Proxy                         = 305,
    SC_307_Temporary_Redirect                = 307,
    SC_400_Bad_Request                       = 400,
    SC_401_Unauthorized                      = 401,
    SC_402_Payment_Required                  = 402,
    SC_403_Forbidden                         = 403,
    SC_404_Not_Found                         = 404,
    SC_405_Method_Not_Allowed                = 405,
    SC_406_Not_Acceptable                    = 406,
    SC_407_Proxy_Authentication_Required     = 407,
    SC_408_Request_Timeout                   = 408,
    SC_409_Conflict                          = 409,
    SC_410_Gone                              = 410,
    SC_411_Length_Required                   = 411,
    SC_412_Precondition_Failed               = 412,
    SC_413_Request_Entity_Too_Large          = 413,
    SC_414_Request_URI_Too_Large             = 414,
    SC_415_Unsupported_Media_Type            = 415,
    SC_416_Requested_Range_Not_Satisfiable   = 416,
    SC_417_Expectation_Failed                = 417,
    SC_500_Internal_Server_Error             = 500,
    SC_501_Not_Implemented                   = 501,
    SC_502_Bad_Gateway                       = 502,
    SC_503_Service_Unavailable               = 503,
    SC_504_Gateway_Timeout                   = 504,
    SC_505_HTTP_Version_Not_Supported        = 505 
  };

public:
  AHTTPResponseHeader();
  AHTTPResponseHeader(const AString& strHeader);
  AHTTPResponseHeader(const AHTTPResponseHeader&);
  AHTTPResponseHeader operator=(const AHTTPResponseHeader&);
  virtual ~AHTTPResponseHeader();

  /*!
  AEmittable and AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;

  /*!
  Clear header
  */
  virtual void clear();

  /*!
  Gets reason phrase for a status code (as per RFC-2616)
  Returns empty string if not found
  */
  static const AString getStatusCodeReasonPhrase(int statusCode);
  
  /*!
  Checks if status code is valid (as per RFC-2616)
  */
  static bool isValidStatusCode(int statusCode);

  /*!
  Access
  */
  int getStatusCode() const;

  /*!
  Set response code and optional description
  */
  void setStatusCode(AHTTPResponseHeader::STATUS_CODES eStatusCode);

  /*!
  Set response code and optional description
  Checks validity amd if status code is not a valid one (as per RFC2616),
    you MUST provide a reason
  */
  void setStatusCode(int statusCode, const AString& reason = AConstant::ASTRING_EMPTY);

  /*!
  Get current reason phrase (empty of default to be used)
  */
  const AString& getReasonPhrase() const;

  /*!
  Response status reason phrase
  */
  void setReasonPhrase(const AString& reason);

  /*!
  Access to cookies
  */
  const ACookies& getCookies() const;
  ACookies& useCookies();

  /*!
  Set the last modified time
  */
  void setLastModified(const ATime&);

protected:
  //a_First line handled by respnse
  //a_Format: [HTTPVERSION] [SP] [STATUSCODE] [SP] [STATUSDESCRIPTION]
  virtual bool _parseLineZero();

  //a_Handle pairs response specific
  virtual bool _handledByChild(const ANameValuePair &nvPair);  

private:
  int      mi_StatusCode;
  AString  mstr_ReasonPhrase;
  ACookies mcookies_Response;

  void _copy(const AHTTPResponseHeader&);
public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AHTTPResponseHeader_HPP__
