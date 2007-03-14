#ifndef INCLUDED__AContentType_Form_HPP__
#define INCLUDED__AContentType_Form_HPP__

#include "apiABase.hpp"
#include "AContentTypeInterface.hpp"
#include "AQueryString.hpp"

class ANameValuePair;

class ABASE_API AContentType_Form : public AContentTypeInterface
{
public:
  //a_Ctors and dtor
  AContentType_Form();
  virtual ~AContentType_Form() {}

  //a_Guaranteed by the base
  virtual void clear();
  virtual void parse();
  virtual void emit(AOutputBuffer&, size_t indent = AConstant::npos) const;  
  virtual void toAFile(AFile& fileIn) const;  //a_Overridden here to avoid emit to buffer then write to file, doing it in 1 step

  AQueryString& useForm() { return m_Form; }

  virtual size_t getContentLength(bool recalculate = true);

private:
  static const AString CONTENT_TYPE;  // text/html

  AQueryString m_Form;
};


#endif // __AForm_HPP__
