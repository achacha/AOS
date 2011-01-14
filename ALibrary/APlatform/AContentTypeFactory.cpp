/*
Written by Alex Chachanashvili

$Id: AContentTypeFactory.cpp 259 2008-11-04 22:02:51Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AString.hpp"
#include "AContentType_Binary.hpp"
#include "AContentType_Form.hpp"
#include "AContentType_TextXml.hpp"
#include "AContentTypeFactory.hpp"
#include "AHTTPHeader.hpp"

AContentTypeInterface* AContentTypeFactory::createContentTypeDocument(const AHTTPHeader& hdr)
{
  AString str;
  if (hdr.get(AHTTPHeader::HT_ENT_Content_Type, str))
  {
    AContentTypeInterface *pDoc = createContentTypeDocument(str);
    pDoc->parseHTTPHeader(hdr);
    return pDoc;
  }
  else
    return NULL;
}

AContentTypeInterface *AContentTypeFactory::createContentTypeDocument(const AString& strType)
{
  AContentTypeInterface *pDoc = NULL;
  if (!strType.compareNoCase(AContentType_TextXml::CONTENT_TYPE)) pDoc = new AContentType_TextXml();
  else if (!strType.compareNoCase(AContentType_Form::CONTENT_TYPE)) pDoc = new AContentType_Form();
  else pDoc = new AContentType_Binary();     //a_Default case is always binary data

  return pDoc;
}
