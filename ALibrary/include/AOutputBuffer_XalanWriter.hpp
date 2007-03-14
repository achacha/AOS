#ifndef INCLUDED__AOutputBuffer_XalanWriter_HPP__
#define INCLUDED__AOutputBuffer_XalanWriter_HPP__

#include "apiAXsl.hpp"
#include "AOutputBuffer.hpp"
#include "AOutputBuffer_XalanOutputStream.hpp"
#include <xalanc/XMLSupport/XMLSupportDefinitions.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xalanc/XalanDOM/XalanDOMString.hpp>
#include <xalanc/PlatformSupport/DOMStringHelper.hpp>
#include <xalanc/PlatformSupport/Writer.hpp>
#include <xalanc/PlatformSupport/XalanMessageLoader.hpp>
#include <xalanc/PlatformSupport/XalanOutputStream.hpp>

class AOutputBuffer_XalanWriter : public XALAN_CPP_NAMESPACE::Writer 
{
public:
  AOutputBuffer_XalanWriter(AOutputBuffer_XalanOutputStream&);
  virtual ~AOutputBuffer_XalanWriter();

  /*!
  Writer interface
  */
  /**
   * Close the stream
   */
  virtual void close();

  /**
   * Flush the stream
   */
  virtual void flush();

  /**
   * Get the stream associated with the writer...
   */
  virtual XALAN_CPP_NAMESPACE::XalanOutputStream* getStream();

  /**
   * Get the stream associated with the writer...
   */
  virtual const XALAN_CPP_NAMESPACE::XalanOutputStream* getStream() const;

  /**
   * Writes a string
   * 
   * @param   s         string to write
   * @param   theOffset starting offset in string to begin writing, default 0
   * @param   theLength number of characters to write. If the length is npos, then the array is assumed to be null-terminated.
   */
  virtual void
  write(
      const char* s,
      size_t theOffset = 0,
      size_t theLength = npos);

  /**
   * Writes a string
   * 
   * @param   s         string to write
   * @param   theOffset starting offset in string to begin writing, default 0
   * @param   theLength number of characters to write. If the length is XalanDOMString::npos, then the array is assumed to be null-terminated.
   */
  virtual void
  write(
      const XALAN_CPP_NAMESPACE::XalanDOMChar* s,
      XALAN_CPP_NAMESPACE::XalanDOMString::size_type theOffset = 0,
      XALAN_CPP_NAMESPACE::XalanDOMString::size_type theLength = XALAN_CPP_NAMESPACE::XalanDOMString::npos);

  /**
   * Writes a character
   * 
   * @param   c         character to write
   */
  virtual void write(XALAN_CPP_NAMESPACE::XalanDOMChar c);

  /**
   * Writes a string
   * 
   * @param   s         string to write
   * @param   theOffset starting offset in string to begin writing, default 0
   * @param   theLength number of characters to write. If the length is XalanDOMString::npos,  then the entire string is printed.
   */
  virtual void
  write(
      const XALAN_CPP_NAMESPACE::XalanDOMString&  s,
      XALAN_CPP_NAMESPACE::XalanDOMString::size_type theOffset = 0,
      XALAN_CPP_NAMESPACE::XalanDOMString::size_type theLength = XALAN_CPP_NAMESPACE::XalanDOMString::npos);

private:
  AOutputBuffer_XalanOutputStream& m_OutputStream;
};

#endif //INCLUDED__AOutputBuffer_XalanWriter_HPP__
