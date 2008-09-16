/*
Written by Alex Chachanashvili

$Id: AOSContext.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AContextUtils_HPP__
#define INCLUDED__AContextUtils_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContext.hpp"

class AXmlElement;
class AString;
class AOutputBuffer;

/*!
Useful AOSContext utilities
*/
class AOS_BASE_API AOSContextUtils
{
public:
  static const AString ATTR_REFERENCE;

public:
  /*!
  \verbatim
  Checks element for reference="" attribute
  If SESSION will extract content at path from session data
  If MODEL will extract content at path from the model
  Else will get the element's content directly

  If we have <foo reference="SESSION">user/data</foo>
  Then in context's session data user/data is looked up and content copied to result

  If we have <foo reference="CONTEXT">request/url</foo>
  Then in context model request/url is looked up and content copied to result

  If we have <foo>some/path/here</foo>
  Then we copy some/path/here to result without lookup
  \endverbatim

  @param context AOSContext
  @param element which contains the reference/path
  @param result of the call appended
  @return true if element is found, reference was valid and content found
  */
  static bool getContentWithReference(AOSContext& context, const AXmlElement& element, AOutputBuffer& result);
};

#endif // INCLUDED__AContextUtils_HPP__
