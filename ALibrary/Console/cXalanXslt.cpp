#include <xalanc/Include/PlatformDefinitions.hpp>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/XalanTransformer/XalanParsedSource.hpp>
#include <xalanc/XalanTransformer/XalanCompiledStylesheet.hpp>

#pragma comment(lib, "Q:\\Xalan-C_1D.lib")
#pragma comment(lib, "Q:\\xerces-c_2D.lib")

int main(int argc, char** argv)
{
  XALAN_USING_STD(cerr)
  XALAN_USING_STD(cout)
  XALAN_USING_STD(endl)

    if (argc < 3 || argc > 4)
	{
		cerr << "Usage: XalanTransform XMLFileName XSLFileName [OutFileName]" << endl;
		return -1;
	}

	int		theResult = -1;

	try
	{
		XALAN_USING_XERCES(XMLPlatformUtils)
		XALAN_USING_XALAN(XalanTransformer)
    XALAN_USING_XALAN(XalanParsedSource)
    XALAN_USING_XALAN(XalanCompiledStylesheet)
    XALAN_USING_XALAN(XSLTResultTarget)

		// Call the static initializer for Xerces.
		XMLPlatformUtils::Initialize();

		// Initialize Xalan.
		XalanTransformer::initialize();

		{
			// Create a XalanTransformer.
			XalanTransformer theXalanTransformer;

      XalanParsedSource *pxml;
      XalanCompiledStylesheet *pxsl;
      XSLTResultTarget result(cout);

      if (theXalanTransformer.parseSource(argv[1], pxml))
      {
        cout << "Unable to parse XML: " << argv[1] << endl;
      }

      if (theXalanTransformer.compileStylesheet(argv[2], pxsl))
      {
        cout << "Unable to parse XSL: " << argv[2] << endl;
      }

      if (argc == 4)
			{
				// Do the transform.
				theResult = theXalanTransformer.transform(*pxml, pxsl, result);
        
				if(theResult != 0)
				{
					cerr << "Error: " << theXalanTransformer.getLastError() << endl;
				}
			}
			else
			{
				// Do the transform.
				theResult = theXalanTransformer.transform(*pxml, pxsl, cout);
        
				if(theResult != 0)
				{
					cerr << "Error: " << theXalanTransformer.getLastError() << endl;
				}
			}
		}

		// Terminate Xalan...
		XalanTransformer::terminate();

		// Terminate Xerces...
		XMLPlatformUtils::Terminate();

		// Clean up the ICU, if it's integrated...
		XalanTransformer::ICUCleanUp();
	}
	catch(...)
	{
		cerr << "An unknown error occurred!" << endl;
	}

	return theResult;
}
