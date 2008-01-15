#include "pchABase.hpp"
#include "AString.hpp"
#include "AEventVisitor.hpp"
#include "AContentType_Binary.hpp"
#include "AContentType_Form.hpp"
#include "AContentType_TextXml.hpp"
#include "AContentType_TextHtml.hpp"
#include "AContentTypeFactory.hpp"
#include "AHTTPHeader.hpp"

AContentTypeInterface* AContentTypeFactory::createContentTypeDocument(const AHTTPHeader& hdr)
{
  AString str;
  if (hdr.getPairValue(AHTTPHeader::HT_ENT_Content_Type, str))
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
  if (!strType.compareNoCase(AContentType_TextHtml::CONTENT_TYPE)) pDoc = new AContentType_TextHtml();
  else if (!strType.compareNoCase(AContentType_TextXml::CONTENT_TYPE)) pDoc = new AContentType_TextXml();
  else if (!strType.compareNoCase(AContentType_Form::CONTENT_TYPE)) pDoc = new AContentType_Form();
  else pDoc = new AContentType_Binary();     //a_Default case is always binary data

  return pDoc;
}
