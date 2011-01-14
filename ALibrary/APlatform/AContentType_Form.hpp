/*
Written by Alex Chachanashvili

$Id: AContentType_Form.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED__AContentType_Form_HPP__
#define INCLUDED__AContentType_Form_HPP__

#include "apiAPlatform.hpp"
#include "AContentTypeInterface.hpp"
#include "AQueryString.hpp"

class ANameValuePair;

class APLATFORM_API AContentType_Form : public AContentTypeInterface
{
public:
  static const AString CONTENT_TYPE;  // application/x-www-form-urlencoded

public:
  //a_Ctors and dtor
  AContentType_Form();
  virtual ~AContentType_Form() {}

  //a_Guaranteed by the base
  virtual void clear();
  virtual void parse();
  virtual void emit(AOutputBuffer&) const;  
  virtual void toAFile(AFile& fileIn) const;  //a_Overridden here to avoid emit to buffer then write to file, doing it in 1 step

  AQueryString& useForm();

  virtual size_t getContentLength(bool recalculate = true);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AQueryString m_Form;
};


#endif // INCLUDED__AContentType_Form_HPP__
