/*
Written by Alex Chachanashvili

$Id: AOSAdmin.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSAdminCommandContext_HPP__
#define INCLUDED__AOSAdminCommandContext_HPP__

#include "apiAOS_Base.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AXmlDocument.hpp"

class AOSAdminCommandContext
{
public:
  //! root element name of the model
  static const AString ELEMENT;

  //! XML dump parameter
  static const AString PARAM_XML;

public:
  /*!
  Create new context
  
  @param request HTTP header
  @param response HTTP header
  @param output buffer
  */
  AOSAdminCommandContext(
    AHTTPRequestHeader& request, 
    AHTTPResponseHeader& response,
    AOutputBuffer&output
  );

  /*!
  XML model root element
  */
  AXmlDocument& useModel();

  /*!
  HTTP request header
  */
  AHTTPRequestHeader& useRequestHeader();

  /*!
  HTTP response header
  */
  AHTTPResponseHeader& useResponseHeader();

  /*!
  Output buffer
  */
  AOutputBuffer& useOutput();

  /*!
  If force XML is on
  */
  bool isForceXml() const;

private:
  AXmlDocument m_Model;
  AHTTPRequestHeader& m_RequestHeader;
  AHTTPResponseHeader& m_ResponseHeader;
  AOutputBuffer& m_Output;
};

#endif // INCLUDED__AOSAdminCommandContext_HPP__
