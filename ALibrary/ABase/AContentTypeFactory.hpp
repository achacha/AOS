/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AContentTypeFactory_HPP__
#define INCLUDED__AContentTypeFactory_HPP__

#include "apiABase.hpp"

class AString;
class AContentTypeInterface;
class AHTTPHeader;

/**
  "text/html" creates AContentType_TextHtml
  "application/x-www-form-urlencoded" creates AContentType_Form
  else creates AContentType_Binary
*/
class ABASE_API AContentTypeFactory
{
public:
  /*
  Object created by these methods are owned and to be released by the caller.
  NULL returned if there is no Content-Type specified
  */
  static AContentTypeInterface *createContentTypeDocument(const AHTTPHeader& hdr);
  static AContentTypeInterface *createContentTypeDocument(const AString& strType);
};

#endif //INCLUDED__AContentTypeFactory_HPP__
