/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAXsl.hpp"
#include "AOutputBuffer_XalanWriter.hpp"
#include "AException.hpp"

AOutputBuffer_XalanWriter::AOutputBuffer_XalanWriter(AOutputBuffer_XalanOutputStream& target) :
  m_OutputStream(target)
{
}

AOutputBuffer_XalanWriter::~AOutputBuffer_XalanWriter()
{
}

void AOutputBuffer_XalanWriter::close()
{
}

void AOutputBuffer_XalanWriter::flush()
{
}

/**
  * Get the stream associated with the writer...
  */
XALAN_CPP_NAMESPACE::XalanOutputStream* AOutputBuffer_XalanWriter::getStream()
{
  return &m_OutputStream;
}

/**
  * Get the stream associated with the writer...
  */
const XALAN_CPP_NAMESPACE::XalanOutputStream* AOutputBuffer_XalanWriter::getStream() const
{
  return &m_OutputStream;
}

/**
  * Writes a string
  * 
  * @param  s         string to write
  * @param  theOffset starting offset in string to begin writing, default 0
  * @param  theLength number of characters to write. If the length is npos, then the array is assumed to be null-terminated.
  */
void AOutputBuffer_XalanWriter::write(
  const char* s,
  size_t theOffset, // = 0
  size_t theLength  // = npos
)
{
  m_OutputStream.useOutputBuffer().append(s+theOffset, theLength);
}

/**
  * Writes a string
  * 
  * @param  s         string to write
  * @param  theOffset starting offset in string to begin writing, default 0
  * @param  theLength number of characters to write. If the length is XalanDOMString::npos, then the array is assumed to be null-terminated.
  */
void AOutputBuffer_XalanWriter::write(
  const XALAN_CPP_NAMESPACE::XalanDOMChar* s,
  XALAN_CPP_NAMESPACE::XalanDOMString::size_type theOffset, // = 0,
  XALAN_CPP_NAMESPACE::XalanDOMString::size_type theLength // = XALAN_CPP_NAMESPACE::XalanDOMString::npos
)
{
  //Output UNICODE as ASCII, UTF-8 is only supported
  for (XALAN_CPP_NAMESPACE::XalanDOMString::size_type t=theOffset; t<theLength; ++t)
    m_OutputStream.useOutputBuffer().append((char)s[t]);
}

/**
  * Writes a character
  * 
  * @param  c         character to write
  */
void AOutputBuffer_XalanWriter::write(XALAN_CPP_NAMESPACE::XalanDOMChar c)
{
  m_OutputStream.useOutputBuffer().append((char)c);
}

/**
  * Writes a string
  * 
  * @param  s         string to write
  * @param  theOffset starting offset in string to begin writing, default 0
  * @param  theLength number of characters to write. If the length is XalanDOMString::npos,  then the entire string is printed.
  */
void AOutputBuffer_XalanWriter::write(
  const XALAN_CPP_NAMESPACE::XalanDOMString& s,
  XALAN_CPP_NAMESPACE::XalanDOMString::size_type theOffset, // = 0
  XALAN_CPP_NAMESPACE::XalanDOMString::size_type theLength // = XALAN_CPP_NAMESPACE::XalanDOMString::npos
)
{
  //Output UNICODE as ASCII, UTF-8 is only supported
  for (XALAN_CPP_NAMESPACE::XalanDOMString::size_type t=theOffset; t<theLength; ++t)
    m_OutputStream.useOutputBuffer().append((char)s.at(t));
}
