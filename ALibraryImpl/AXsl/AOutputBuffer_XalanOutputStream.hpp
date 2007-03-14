#ifndef INCLUDED__AOutputBuffer_XalanOutputStream_HPP__
#define INCLUDED__AOutputBuffer_XalanOutputStream_HPP__

#include "apiAXsl.hpp"
#include "AOutputBuffer.hpp"
#include <xalanc/XMLSupport/XMLSupportDefinitions.hpp>
#include <xalanc/PlatformSupport/XalanOutputStream.hpp>

class AOutputBuffer_XalanOutputStream : public XALAN_CPP_NAMESPACE::XalanOutputStream 
{
public:
  AOutputBuffer_XalanOutputStream(
    AOutputBuffer&, 
    XALAN_CPP_NAMESPACE::MemoryManagerType& theManager = XALAN_CPP_NAMESPACE::XalanMemMgrs::getDefaultXercesMemMgr()
  );
  virtual ~AOutputBuffer_XalanOutputStream();

	/**
	 * Write the data in the buffer
	 *
	 * @param theBuffer The data to write
	 * @param theBufferLength The length of theBuffer.
	 */
  virtual void writeData(
    const char *theBuffer, 
    XALAN_CPP_NAMESPACE::XalanOutputStream::size_type theBufferLength
  );

	/**
	 * Flush the stream.
	 */
  virtual void doFlush();

  /*!
  Access to output buffer
  */
  AOutputBuffer& useOutputBuffer() { return m_OutputBuffer; }

private:
  AOutputBuffer& m_OutputBuffer;
};

#endif //INCLUDED__AOutputBuffer_XalanOutputStream_HPP__
