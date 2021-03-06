/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"

#ifdef __WINDOWS__
#include "AOSOutput_MsXslt.hpp"
#include "ASystemException.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AOSServices.hpp"

#define _WIN32_DCOM 
#import <msxml6.dll>

const AString AOSOutput_MsXslt::CLASS("Xslt");

const AString& AOSOutput_MsXslt::getClass() const
{
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
      ASWNL("1:Enable template caching, 0:Clear only, -1:Disable and clear"),
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
        if (str.equals("-1",2))
        {
          //a_Clear and disable
          m_IsCacheEnabled = false;
          
          ALock lock(m_CacheSync);
          m_Dox.clear();
        }
        else if (str.equals("0",1))
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
  m_IsCacheEnabled = m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/base-modules/AOSOutput_MsXslt/cache/enabled",51), true);
}

AOSOutput_MsXslt::~AOSOutput_MsXslt()
{
  DocContainer::iterator it = m_Dox.begin();
  while (it != m_Dox.end())
  {
//a_This is now being done by MS code: ((MSXML2::IXMLDOMDocument3Ptr *)((*it).second).m_ComPtr)->Release();
    delete (*it).second.m_ComPtr;
    ++it;
  }
}

void AOSOutput_MsXslt::init()
{
  CoInitializeEx(NULL,COINIT_MULTITHREADED);
}

void AOSOutput_MsXslt::deinit()
{
  CoUninitialize();
}

AOSOutput_MsXslt::XslDocHolder *AOSOutput_MsXslt::_readXslFile(const AString& filename)
{
  MSXML2::IXMLDOMDocument3Ptr *p = new MSXML2::IXMLDOMDocument3Ptr;
  *p = NULL;

  if (filename.isEmpty())
  {
    delete p;
    ATHROW_EX(this, AException::InvalidParameter, AString("Empty filename"));
  }

  HRESULT hr = (*p).CreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
  if (FAILED(hr))
  {
    delete p;
    ATHROW_LAST_OS_ERROR(this);
  }

  (*p)->put_async(VARIANT_FALSE);  
  (*p)->put_validateOnParse(VARIANT_FALSE);
  //(*p)->put_resolveExternals(VARIANT_FALSE);
  if (! (*p)->load((const _bstr_t)filename.c_str()) )
  {
    BSTR bstr;
    long n;
    ARope rope("Unable to parse XSL: ");

    (*p)->parseError->get_reason(&bstr);
    rope.append((LPCSTR)_bstr_t(bstr, false));

    (*p)->parseError->get_line(&n);
    rope.append(" line=");
    rope.append(AString::fromSize_t(n));
    
    (*p)->parseError->get_linepos(&n);
    rope.append(" line.pos=");
    rope.append(AString::fromSize_t(n));

    (*p)->parseError->get_filepos(&n);
    rope.append(" file.pos=");
    rope.append(AString::fromSize_t(n));

    rope.append(AConstant::ASTRING_EOL);

    ((MSXML2::IXMLDOMDocument3Ptr *)p)->Release();
    delete p;
    ATHROW_EX(this, AException::OperationFailed, rope);
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
  const AXmlElement *peFilename = context.getOutputParams().findElement(AOS_BaseModules_Constants::FILENAME);
  if (!peFilename)
  {
    context.useEventVisitor().addEvent(ASWNL("AOSOutput_File: Unable to find 'path' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }

  //a_Get filename requested
  AString str(1536, 1024);
  peFilename->emitContent(str);

  AAutoPtr<AFilename> pFilename;
  AString strBase;
  if (peFilename->getAttributes().get(AOS_BaseModules_Constants::BASE, strBase))
  {
    if (strBase.equals(ASW("data",4)))
      pFilename.reset(new AFilename(m_Services.useConfiguration().getAosBaseDataDirectory(), str, false));
    else if (strBase.equals(ASW("dynamic",7)))
      pFilename.reset(new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory(), str, false));
    else if (strBase.equals(ASW("absolute",8)))
      pFilename.reset(new AFilename(str, false));
  }

  //a_Static is the default
  if (!pFilename)
    pFilename.reset(new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory(), str, false));
  
  if (!AFileSystem::exists(*pFilename))
  {
    context.addError(ASWNL("AOSOutput_File"), ARope("File not found: ",16)+*pFilename);
    return AOSContext::RETURN_ERROR;
  }

  //a_See if extension for mime type set
  AString ext;
  if (context.getOutputParams().emitContentFromPath(AOS_BaseModules_Constants::MIME_EXTENSION, ext))
  {
    m_Services.useConfiguration().setMimeTypeFromExt(ext, context);
  }
  else
  {
    //a_Set content type based on file extension
    AString ext;
    pFilename->emitExtension(ext);
    m_Services.useConfiguration().setMimeTypeFromExt(ext, context);
  }

  MSXML2::IXMLDOMDocument3Ptr *p = NULL;
  XslDocHolder *pXslDocHolder = _getXslDocHolder(*pFilename);

  //a_Document exists in cache
  p = (MSXML2::IXMLDOMDocument3Ptr *)pXslDocHolder->m_ComPtr;
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
    MSXML2::IXMLDOMDocument3Ptr pXMLDoc = NULL;
    HRESULT h = pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
    if (FAILED(h))
      ATHROW_LAST_OS_ERROR(this);

    AString str;
    rope.emit(str);
    const _bstr_t bstrXml = str.c_str();
    if (! pXMLDoc->loadXML(bstrXml) )
    {
      BSTR bstr;
      long n;
      ARope rope("Unable to parse XML: ");

      pXMLDoc->parseError->get_reason(&bstr);
      rope.append((LPCSTR)_bstr_t(bstr, false));

      pXMLDoc->parseError->get_line(&n);
      rope.append(" line=");
      rope.append(AString::fromSize_t(n));
      
      pXMLDoc->parseError->get_linepos(&n);
      rope.append(" line.pos=");
      rope.append(AString::fromSize_t(n));

      pXMLDoc->parseError->get_filepos(&n);
      rope.append(" file.pos=");
      rope.append(AString::fromSize_t(n));

      rope.append(AConstant::ASTRING_EOL);

      ATextConverter::convertStringToHexDump(str, rope);
      context.addError(getClass(), rope);
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
  context.useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html; charset=utf-8",23));

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

#endif // __WINDOWS__