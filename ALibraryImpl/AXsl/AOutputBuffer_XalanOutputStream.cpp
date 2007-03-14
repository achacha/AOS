#include "pchAXsl.hpp"
#include "AOutputBuffer_XalanOutputStream.hpp"
#include "AException.hpp"

AOutputBuffer_XalanOutputStream::AOutputBuffer_XalanOutputStream(
  AOutputBuffer& target,
  XALAN_CPP_NAMESPACE::MemoryManagerType& theManager // = XALAN_CPP_NAMESPACE::XalanMemMgrs::getDefaultXercesMemMgr()
) :
  XALAN_CPP_NAMESPACE::XalanOutputStream(theManager),
  m_OutputBuffer(target)
{
}

AOutputBuffer_XalanOutputStream::~AOutputBuffer_XalanOutputStream()
{
}

void AOutputBuffer_XalanOutputStream::writeData(
  const char *theBuffer, 
  XALAN_CPP_NAMESPACE::XalanOutputStream::size_type theBufferLength
)
{
  m_OutputBuffer.append(theBuffer, theBufferLength);
}

void AOutputBuffer_XalanOutputStream::doFlush()
{
}
