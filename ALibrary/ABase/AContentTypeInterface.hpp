#ifndef __AContentTypeInterface_HPP__
#define __AContentTypeInterface_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AFile_AString.hpp"
#include "AEmittable.hpp"

class AFile;
class AUrl;
class AHTTPHeader;

class ABASE_API AContentTypeInterface : public ASerializable, public ADebugDumpable
{
public:
  AContentTypeInterface(const AString& type);
  virtual ~AContentTypeInterface();

  /*!
  Gives access to the physical data file that can be written to and then parse()'d when done
  Since calling parse() for a child class may result in different output
  */
  AFile& useData();

  /*!
  Parse data
  */
  virtual void parse() = 0;
  
  /*!
  Clear
  */
  virtual void clear();
  
  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;  

  /*!
  ASerialization
  */
  virtual void toAFile(AFile& fileIn) const;
  virtual void fromAFile(AFile& fileIn);

  /*!
  Use of the HTTP header
  Gathers needed info from the header about how to read the document
  */
  virtual void parseHTTPHeader(const AHTTPHeader& header);

  /*!
  Access the type this object was created for by the factory
  Can be overridden
  */
  const AString& getContentType() const;
  void setContentType(const AString& strType);

  /*!
  Access to the content length AFTER calling parseHTTPResponseHeader
  */
  virtual size_t getContentLength();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //a_Base class needs to know what content type it is based on child class
  AContentTypeInterface() {}

  //a_Raw page data
  AFile_AString m_Data;

  //a_Length of content about to be read (used during header read before data read)
  size_t m_ContentLength;

  //a_The type this object is supporting (set by AContentTypeFactory)
  AString mstr_ContentType;

private:
  AContentTypeInterface(const AContentTypeInterface&) {}
};

#endif


