/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"

#ifdef AOS__USE_MSXML6__
#include "AOSOutput_MsXslt.hpp"
#include "ASystemException.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AOSServices.hpp"

#import <msxml6.dll>

const AString& AOSOutput_MsXslt::getClass() const
{
  static const AString CLASS("Xslt");
  return CLASS;
}

void AOSOutput_MsXslt::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  {
    AXmlElement& elem = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("cache",5));

    adminAddPropertyWithAction(
      elem, 
      ASW("enabled",7), 
      AString::fromBool(m_IsCacheEnabled),
      ASW("Update",6), 
      ASWNL("1:Enable template caching, 0:Disable and clear, -1:Clear only"),
      ASW("Set",3)
    );

    adminAddProperty(elem, ASW("size",4), AString::fromSize_t(m_Dox.size()));
  }
}

void AOSOutput_MsXslt::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    //a_Toggle cache state and clear when disabled
    if (str.equals(ASW("AOSOutputExecutor.Xslt.cache.enabled",36)))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        if (str.equals("0"))
        {
          //a_Clear and disable
          m_IsCacheEnabled = false;
          
          ALock lock(m_CacheSync);
          m_Dox.clear();
        }
        else if (str.equals("-1"))
        {
          ALock lock(m_CacheSync);
          m_Dox.clear();
        }
        else
        {
          //a_Enable
          m_IsCacheEnabled = true;
        }
      }
    }
  }
}

AOSOutput_MsXslt::AOSOutput_MsXslt(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
	CoInitialize(NULL);

  m_IsCacheEnabled = m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/base-modules/AOSOutput_MsXslt/cache",43), true);
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
}

void AOSOutput_MsXslt::deinit()
{
}

AOSOutput_MsXslt::XslDocHolder *AOSOutput_MsXslt::_readXslFile(const AString& filename)
{
  MSXML2::IXMLDOMDocument2Ptr *p = new MSXML2::IXMLDOMDocument2Ptr;
  *p = NULL;

  if (filename.isEmpty())
  {
    delete p;
    ATHROW_EX(this, AException::InvalidParameter, AString("Empty filename"));
  }

  HRESULT hr = (*p).CreateInstance(__uuidof(MSXML2::DOMDocument60));
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

AOSOutput_MsXslt::XslDocHolder *AOSOutput_MsXslt::_getXslDocHolder(const AFilename& filename)
{
  XslDocHolder *pXslDocHolder = NULL;
  AString xsltPathName;
  filename.emit(xsltPathName);

  ALock lock(m_CacheSync);
  DocContainer::iterator it = m_Dox.find(xsltPathName);
  if (
    it == m_Dox.end() 
    || m_IsCacheEnabled
  )
  {
    pXslDocHolder = _readXslFile(xsltPathName);
  }
  else
  {
    pXslDocHolder = (XslDocHolder *)(&(*it).second);
  }
  
  AASSERT(this, pXslDocHolder);
  return pXslDocHolder;
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
  XslDocHolder *pXslDocHolder = _getXslDocHolder(xsltFile);

  //a_Document exists in cache
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
    HRESULT h = pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
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

#endif //AOS__USE_MSXML6__