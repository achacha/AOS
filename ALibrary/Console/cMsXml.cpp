
#import <msxml4.dll>
using namespace MSXML2;

#include "AString.hpp"

void dump_com_error(_com_error &e)
{
	_bstr_t err = _bstr_t("Error\n") + _bstr_t( e.Error()) + _bstr_t("\n") + e.ErrorMessage() + e.Source()+ _bstr_t("\n") + e.Description() ;
  std::cout << err << std::endl;
}

int main()
{
	IXMLDOMDocument2Ptr pXMLDoc = NULL;
	IXMLDOMDocument2Ptr pXSLDoc = NULL;
  IXMLDOMDocument2Ptr pXMLOut = NULL;

  //init xml doc, xsd doc and schemacache obj
	CoInitialize(NULL);
	try{
		HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument40)),	
		        hr1 = pXSLDoc.CreateInstance(__uuidof(DOMDocument40)),	
            hr2 = pXMLOut.CreateInstance(__uuidof(DOMDocument40));
    if (FAILED(hr) || FAILED(hr1) || FAILED(hr2))
    {
      std::cout << "Failed to created output DOM object." << std::endl;
    }

		//a_Load XML document
		pXMLDoc->async =  VARIANT_FALSE;                      //set async to false
    if (! pXMLDoc->load(_bstr_t("Q:\\foo.xml")) )
    {
      std::cout << "Failed to created XML DOM object." << std::endl;
    }

		//a_Load XSL document
		pXSLDoc->async =  VARIANT_FALSE;                      //set async to false
    if (! pXSLDoc->load(_bstr_t("Q:\\foo.xsl")) )
    {
      std::cout << "Failed to created XSL DOM object." << std::endl;
    }


    // Transform the XSLT to an XML string.
    _bstr_t xmlStr = pXMLDoc->transformNode(pXSLDoc);

    //Always remember to check for parse errors.
    if(pXMLDoc->parseError->errorCode != 0)
    {
      std::cout << "Failed to transformNode: " << (LPCSTR)pXMLDoc->parseError->Getreason() << std::endl;
    }
    else 
    {
      std::cout << "RESULT:\n" << (LPCSTR)xmlStr << std::endl;
    }
    std::cout << std::endl;

    // Transform the XSLT to a DOM object.
    std::cout << pXMLDoc->xml << std::endl;
    std::cout << pXSLDoc->xml << std::endl;
    hr = pXMLDoc->transformNodeToObject(pXSLDoc, pXMLOut.GetInterfacePtr());
    std::cout << pXMLOut->xml << std::endl;
    if (FAILED(hr))
    {
      std::cout << "Failed to transformNodeToObject: " << pXMLDoc->parseError->Getreason() << std::endl;
    }
    else
    {
      hr = pXMLOut->save("foo.html");
      if (FAILED(hr))
      {
        std::cout << "Failed to save output DOM to foo.html\n";
      }
      else
      {
        std::cout << "Saved to foo.html, output from transformNodeToObject:\n" << pXMLOut->xml << std::endl;
      }
    }
  }
  catch(_com_error &e)
	{
		dump_com_error(e);
	}
  
  if (pXMLDoc) pXMLDoc.Release();
  if (pXSLDoc) pXSLDoc.Release();
  if (pXMLOut) pXMLOut.Release();
  CoUninitialize();

  return 0;
}
