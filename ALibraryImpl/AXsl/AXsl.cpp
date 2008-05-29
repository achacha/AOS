/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAXsl.hpp"
#include "AXsl.hpp"
#include "AException.hpp"
#include "AOutputBuffer_XalanWriter.hpp"

#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

#ifndef NDEBUG
#  pragma comment(lib, "xerces-c_2D")
#  pragma comment(lib, "Xalan-C_1D")
#else
#  pragma comment(lib, "xerces-c_2")
#  pragma comment(lib, "Xalan-C_1")
#endif

struct AXSL_INTERNAL_DATA
{
  XALAN_CPP_NAMESPACE::XalanTransformer *mp_xalanTransformer;
  typedef std::map<const AString, const XALAN_CPP_NAMESPACE::XalanCompiledStylesheet *> MAP_COMPILED;
  MAP_COMPILED m_CompiledStylesheets; 
};

AXsl::AXsl() :
  mp_InternalData(NULL)
{
	// Call the static initializer for Xerces.
  XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

	// Initialize Xalan.
  XALAN_CPP_NAMESPACE::XalanTransformer::initialize();

  AXSL_INTERNAL_DATA *p = new AXSL_INTERNAL_DATA();
  mp_InternalData = p;
  p->mp_xalanTransformer = new XALAN_CPP_NAMESPACE::XalanTransformer();
}

AXsl::~AXsl()
{
  try
  {
    //a_Memory cleanup
    AXSL_INTERNAL_DATA *p = static_cast<AXSL_INTERNAL_DATA *>(mp_InternalData);
    if (p) delete p->mp_xalanTransformer;
    delete p;

    // Terminate Xalan...
    XALAN_CPP_NAMESPACE::XalanTransformer::terminate();

		// Terminate Xerces...
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

		// Clean up the ICU, if it's integrated...
    XALAN_CPP_NAMESPACE::XalanTransformer::ICUCleanUp();
  }
  catch(...) {}
}

int AXsl::Transform(AEmittable& xml, const AString& xslFilename, AOutputBuffer& target)
{ 
  AXSL_INTERNAL_DATA *pInternalData = static_cast<AXSL_INTERNAL_DATA *>(mp_InternalData);
#ifndef NDEBUG
  if (!pInternalData)
    ATHROW_EX(NULL, AException::ProgrammingError, ASWNL("Internal initialization error"));
  if (!pInternalData->mp_xalanTransformer)
    ATHROW_EX(NULL, AException::ProgrammingError, ASWNL("XSL transformer not initialized"));
#endif

  int result;

  //a_Parse XML
  AString strXml;
  xml.emit(strXml);
  XERCES_CPP_NAMESPACE::MemBufInputSource theInputSource((const XMLByte *)strXml.c_str(), strXml.getSize(), "foobar");
	XALAN_CPP_NAMESPACE::XercesParserLiaison::DOMParserType theParser;
	theParser.parse(theInputSource);
 	XALAN_CPP_NAMESPACE::XercesParserLiaison theParserLiaison;
	XALAN_CPP_NAMESPACE::XercesDOMSupport theDOMSupport;
	const XALAN_CPP_NAMESPACE::XercesDOMWrapperParsedSource theWrapper(
				theParser.getDocument(),
				theParserLiaison,
				theDOMSupport
        );

  //a_Compile XSL if notdone so
  const XALAN_CPP_NAMESPACE::XalanCompiledStylesheet*	theCompiledStylesheet = 0;
  AXSL_INTERNAL_DATA::MAP_COMPILED::iterator it = pInternalData->m_CompiledStylesheets.find(xslFilename);
  if (it == pInternalData->m_CompiledStylesheets.end())
  {
    result = pInternalData->mp_xalanTransformer->compileStylesheet(xslFilename.c_str(), theCompiledStylesheet);
    if (result)
      ATHROW_EX(NULL, AException::OperationFailed, AString(pInternalData->mp_xalanTransformer->getLastError()));
    
    pInternalData->m_CompiledStylesheets[xslFilename] = theCompiledStylesheet;
  }
  else
  {
    theCompiledStylesheet = pInternalData->m_CompiledStylesheets[xslFilename];
  }

  AOutputBuffer_XalanOutputStream theStream(target);
  AOutputBuffer_XalanWriter theWriter(theStream);
  XALAN_CPP_NAMESPACE::XSLTResultTarget theResult(&theWriter);
  result = pInternalData->mp_xalanTransformer->transform(theWrapper, theCompiledStylesheet, theResult);
	if(result)
    ATHROW_EX(NULL, AException::OperationFailed, AString(pInternalData->mp_xalanTransformer->getLastError()));

  return result;
}