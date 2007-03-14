#ifndef __AContentTypeInterface_HPP__
#define __AContentTypeInterface_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"

class AFile;
class AUrl;
class AHTTPHeader;

class ABASE_API AContentTypeInterface : public ASerializable, public ADebugDumpable
{
public:
  AContentTypeInterface();
  AContentTypeInterface(const AString& type);
  virtual ~AContentTypeInterface();

  //a_Access methods to data (may be overridden by derived classes)
  virtual void setData(const AString& strData);
  virtual void addData(const AString& strData);
  
  //a_Gives access to the physical data object (may not be what toString() outputs)
  //a_Since calling parse() for a child class may result in different output
  //a_Modifications 
  inline AString& useData();

  //a_Activation method
  virtual void parse() = 0;
  virtual void clear();
  
  //a_AString representation (ouput may vary based on document type)
  //a_indent is also implementation specific
  virtual void emit(AOutputBuffer&, size_t indent = AConstant::npos) const;  

  //a_AFile use
  virtual void toAFile(AFile& fileIn) const;
  virtual void fromAFile(AFile& fileIn);

  //a_Use of the HTTP header
  //a_Gathers needed info from the header about how to read the document
  virtual void parseHTTPHeader(AHTTPHeader& header);

  //a_Access the type this object was created for by the factory
  const AString& getContentType() const;
  void setContentType(const AString& strType);

  //a_Access to the content length AFTER calling parseHTTPResponseHeader
  virtual size_t getContentLength();

protected:
  //a_Raw page data
  AString mstr_Data;

  //a_Length of content about to be read (used during hedaer read before data read)
  size_t m_ContentLength;

  //a_The type this object is supporting (set by AContentTypeFactory)
  AString mstr_ContentType;

private:
  AContentTypeInterface(const AContentTypeInterface&) {}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif


