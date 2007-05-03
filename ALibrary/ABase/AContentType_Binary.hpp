#ifndef INCLUDED__AContentType_Binary_HPP__
#define INCLUDED__AContentType_Binary_HPP__

#include "apiABase.hpp"
#include "AContentTypeInterface.hpp"

class ASync_EventVisitor;
class AOutputBuffer;

class ABASE_API AContentType_Binary : public AContentTypeInterface
{
public:
  AContentType_Binary();
  virtual ~AContentType_Binary() {}

  //a_Activation methods do nothing, binary data stores all types and knows nothing about them
  virtual void parse() {}

  //a_Represent as AString (in this case a hex dump)
  virtual void emit(AOutputBuffer&, size_t indent = AConstant::npos) const;  

private:
  static const AString CONTENT_TYPE;  // text/html
};

#endif

