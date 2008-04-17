#include "pchAOS_BaseModules.hpp"

#ifdef AOS__USE_MSXML4__
#include "AOSOutput_MsXslt.hpp"
#include "ASystemException.hpp"
#include "AObjectContainer.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AOSServices.hpp"

#import <msxml4.dll>

const AString& AOSOutput_MsXslt::getClass() const
{
  static const AString CLASS("Xslt");
  return CLASS;
}

AOSOutput_MsXslt::AOSOutput_MsXslt(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
	CoInitialize(NULL);
}

AOSOutput_MsXslt::~AOSOutput_MsXslt()
{
  DocContainer::iterator it = m_Dox.begin();
  while (it != m_Dox.end())
  {
    ((MSXML2::IXMLDOMDocument2Ptr *)((*it).second).m_ComPtr)->Release();
    delete (*it).second.m_ComPtr;
    ++it;
  }
  CoUninitialize();
}

void AOSOutput_MsXslt::init()
{
#pragma message("FIX: AOSOutput_MsXslt::init: XSLT loading hardcoded")
//  addXslDocument(ASW("aos", 3), ASW("q:/aos.xsl", 10));
//  addXslDocument(ASW("dada", 4), ASW("q:/dada.xsl", 11));
}

void AOSOutput_MsXslt::deinit()
{
}

AOSOutput_MsXslt::XslDocHolder *AOSOutput_MsXslt::_readXslFile(const AString& filename)
{
  MSXML2::IXMLDOMDocument2Ptr *p = new MSXML2::IXMLDOMDocument2Ptr;
  *p= NULL;

  if (filename.isEmpty())
  {
    delete p;
    ATHROW_EX(this, AException::InvalidParameter, AString("Empty filename"));
  }

  HRESULT hr = (*p).CreateInstance(__uuidof(MSXML2::DOMDocument40));
  if (FAILED(hr))
  {
    delete p;
    ATHROW_LAST_OS_ERROR(this);
  }

	(*p)->async = VARIANT_FALSE;
  if (! (*p)->load((const _bstr_t)filename.c_str()) )
  {
    ((MSXML2::IXMLDOMDocument2Ptr *)p)->Release();
    delete p;
    ATHROW_EX(this, AException::OperationFailed, AString("Unable to parse XSL: ")+filename);
  }

  XslDocHolder holder;
  holder.m_ComPtr = p;
  holder.m_filename = filename;

  m_Dox[filename] = holder;
  m_Services.useLog().append(AString("AOSOutput_MsXslt::addXslDocument: '")+filename+ "'");

  return &(m_Dox[filename]);
}

AOSContext::ReturnCode AOSOutput_MsXslt::execute(AOSContext& context)
{
	CoInitialize(NULL);

  AFilename xsltFile(m_Services.useConfiguration().getAosBaseDataDirectory());
  AString xsltName;
  if (!context.getOutputParams().emitString(ASW("filename", 8), xsltName))
  {
    m_Services.useLog().append("AOSOutput_MsXslt: Unable to find '/output/filename' parameter");
    return AOSContext::RETURN_ERROR;
  }
  xsltFile.join(xsltName, false);

  MSXML2::IXMLDOMDocument2Ptr *p = NULL;
  AString xsltPathName(xsltFile.toAString());
  DocContainer::iterator it = m_Dox.find(xsltPathName);
  XslDocHolder *pXslDocHolder = NULL;
  if (
    it == m_Dox.end() 
    || context.useRequestParameterPairs().exists(ASW("disableXslCache", 15))
  )
  {
    pXslDocHolder = _readXslFile(xsltPathName);
  }
  else
  {
    pXslDocHolder = (XslDocHolder *)(&(*it).second);
  }

  //a_Document exists in cache
  AASSERT(this, pXslDocHolder);
  p = (MSXML2::IXMLDOMDocument2Ptr *)pXslDocHolder->m_ComPtr;
  AASSERT(this, p);

  //a_Emit the context XML document
  ARope rope;
  context.useModelXmlDocument().emit(rope);

  //a_Force XML to file
  if (context.useRequestParameterPairs().exists(ASW("dumpXmlToFile", 13)))
  {
    _dumpToFile(rope);
  }

  try
  {
    MSXML2::IXMLDOMDocument2Ptr pXMLDoc = NULL;
    HRESULT h = pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
    if (FAILED(h))
      ATHROW_LAST_OS_ERROR(this);

    AString str;
    rope.emit(str);
    const _bstr_t bstrXml = str.c_str();
    if (! pXMLDoc->loadXML(bstrXml) )
    {
      context.addError(getClass(), AString("Unable to parse XML:\n ")+str+"\n");
      return AOSContext::RETURN_ERROR;
    }

    //a_Reparse XSL every time if cache disabled
    _bstr_t xmlStr = pXMLDoc->transformNode(*p);

    //Always remember to check for parse errors.
    if(pXMLDoc->parseError->errorCode != 0)
    {
      _dumpToFile(rope);
      context.addError(getClass(), ARope("Failed to transformNode: ",25) + (LPCSTR)pXMLDoc->parseError->Getreason());
      return AOSContext::RETURN_ERROR;
    }
    else 
    {
      context.useOutputBuffer().append((LPCSTR)xmlStr);
    }
  }
  catch(_com_error& e)
	{
   	_bstr_t err = _bstr_t("COM_Error\n") + _bstr_t( e.Error()) + _bstr_t("\n") + e.ErrorMessage() + _bstr_t("\n") + e.Source()+ _bstr_t("\n") + e.Description() ;
    context.addError(getClass(), AString((LPCSTR)err));
    return AOSContext::RETURN_ERROR;
	}

  //a_Add content type, length and other useful response data to response header
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html", 9));

  CoUninitialize();

  return AOSContext::RETURN_OK;
}

void AOSOutput_MsXslt::_dumpToFile(ARope& rope)
{
  ATime time;
  AString errorfile("xslt_error_");
  time.emit(errorfile);
  errorfile.append('_');
  ATextGenerator::generateRandomAlphanum(errorfile, 8);
  errorfile.append(ASW(".xml",4));

  AFile_Physical outfile(errorfile, ASW("w",1));
  rope.emit(outfile);
  outfile.close();
}

#endif //AOS__USE_MSXML4__