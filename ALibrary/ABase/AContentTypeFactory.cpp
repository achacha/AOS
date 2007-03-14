#include "pchABase.hpp"
#include "AString.hpp"
#include "AEventVisitor.hpp"
#include "AContentType_Binary.hpp"
#include "AContentType_Form.hpp"
#include "AContentType_TextHtml.hpp"
#include "AContentTypeFactory.hpp"
#include "AHTTPHeader.hpp"

AContentTypeInterface* AContentTypeFactory::createContentTypeDocument(const AHTTPHeader& hdr)
{
  AString str;
  if (hdr.find(AHTTPHeader::HT_ENT_Content_Type, str))
    return createContentTypeDocument(str);
  else
    return NULL;
}

AContentTypeInterface* AContentTypeFactory::createContentTypeDocument(const AString& strType)
{
  AContentTypeInterface *pDoc = NULL;
  if (!strType.compareNoCase("text/html"))
    pDoc = new AContentType_TextHtml();
  else if (!strType.compareNoCase("application/x-www-form-urlencoded"))
    pDoc = new AContentType_Form();
  else
    pDoc = new AContentType_Binary();     //a_Default case is always binary data

  pDoc->setContentType(strType);
  return pDoc;
}
