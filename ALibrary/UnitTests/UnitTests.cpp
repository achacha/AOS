/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"

#include "UnitTests.hpp"
#include <ATimer.hpp>        //a_Used for timing

bool boolAllTests = false;    //a_Global set to true if all tests are being run to reduce spam

void display()
{
  std::cout << "Usage: UnitTests T0:n0 T1:n1 T2:n2 T3:n3 ... " << std::endl;
  std::cout << "  Tx = test number  nx = number of times to execute same test" << std::endl;
  std::cout << " 0  : RUN ALL TESTS" << std::endl;
  std::cout << " 1  : AString" << std::endl;
  std::cout << " 2  : AString comparisson" << std::endl;
  std::cout << " 3  : AString access" << std::endl;
  std::cout << " 4  : AString searching" << std::endl;
  std::cout << " 5  : AString utility methods" << std::endl;
  std::cout << " 6  : AString modification methods" << std::endl;
  std::cout << " 7  : AStringHashMap" << std::endl;
  std::cout << " 8  : ARope" << std::endl;
  std::cout << " 9  : ARopeDeque" << std::endl;
  std::cout << "10  : AException" << std::endl;
  std::cout << "12  : ATextConverter" << std::endl;
  std::cout << "13  : ATextOdometer" << std::endl;
  std::cout << "14  : AFilename" << std::endl;
  std::cout << "15  : AFile_IOStream" << std::endl;
  std::cout << "16  : AFile_IOStream manipulation" << std::endl;
  std::cout << "17  : AFile_AString" << std::endl;
  std::cout << "19  : AFileSystem" << std::endl;
  std::cout << "20  : AINIProfile" << std::endl;
  std::cout << "25  : ARandomNumberGenerator" << std::endl;
  std::cout << "26  : ATextGenerator" << std::endl;
  std::cout << "27  : AMD5Digest" << std::endl;
  std::cout << "28  : AChecksum" << std::endl;
  std::cout << "29  : ASHA1Digest" << std::endl;
  std::cout << "35  : ANumber" << std::endl;
  std::cout << "36  : ANumber internal (conditional compile)" << std::endl;
  std::cout << "37  : ANumber functionality" << std::endl;
  std::cout << "38  : ANumber unique cases" << std::endl;
  std::cout << "39  : ANameValuePair" << std::endl;
	std::cout << "40  : AFragmentString" << std::endl;
	std::cout << "41  : ABitArray" << std::endl;
  std::cout << "50  : AObject" << std::endl;
  std::cout << "52  : AFile_Socket" << std::endl;
  std::cout << "65  : ATimer" << std::endl;
  std::cout << "67  : ATime" << std::endl;
  std::cout << "70  : ANamePointerPair" << std::endl;
  std::cout << "75  : AXmlAttributes" << std::endl;
  std::cout << "80  : AXmlElement" << std::endl;
  std::cout << "82  : AXmlDocument" << std::endl;
  std::cout << "100 : AUrl" << std::endl;
  std::cout << "105 : ACookie" << std::endl;
  std::cout << "110 : ABasePtrQueue" << std::endl;
  std::cout << "200 : Macros" << std::endl;
  std::cout << std::endl;
}

int executeTest(int iTestNumber)
{
  _fnTEST *pfnX = NULL;
  switch(iTestNumber)
  {
    case   1 : pfnX = ut_AString_General; break;
    case   2 : pfnX = ut_AString_Compare; break;
    case   3 : pfnX = ut_AString_Access; break;
    case   4 : pfnX = ut_AString_Search; break;
    case   5 : pfnX = ut_AString_Utility; break;
    case   6 : pfnX = ut_AString_Modify; break;
    case   7 : pfnX = ut_AStringHashMap_General; break;
    case   8 : pfnX = ut_ARope_General; break;
    case   9 : pfnX = ut_ARopeDeque_General; break;
    case  10 : pfnX = ut_AException_General; break;
    case  12 : pfnX = ut_ATextConverter_General; break;
    case  13 : pfnX = ut_ATextOdometer_General; break;
    case  14 : pfnX = ut_AFilename_General; break;
    case  15 : pfnX = ut_AFileIOStream_General; break;
    case  16 : pfnX = ut_AFileIOStream_Manip; break;
    case  17 : pfnX = ut_AFileAString_General; break;
    case  19 : pfnX = ut_AFileSystem_General; break;
		case  20 : pfnX = ut_AINIProfile_General; break;
		case  25 : pfnX = ut_ARandomNumberGenerator_General; break;
		case  26 : pfnX = ut_ATextGenerator_General; break;
		case  27 : pfnX = ut_AMD5Digest_General; break;
		case  28 : pfnX = ut_AChecksum_General; break;
		case  29 : pfnX = ut_ASHA1Digest_General; break;
    case  35 : pfnX = ut_ANumber_General; break;
    case  36 : pfnX = ut_ANumber_Internal; break;
    case  37 : pfnX = ut_ANumber_Functionality; break;
    case  38 : pfnX = ut_ANumber_Unique; break;
    case  39 : pfnX = ut_ANameValuePair_General; break;
    case  40 : pfnX = ut_AFragmentString_General; break;
    case  41 : pfnX = ut_ABitArray_General; break;
    case  50 : pfnX = ut_AObject_General; break;
    case  52 : pfnX = ut_AFile_Socket_General; break;
    case  65 : pfnX = ut_ATimer_General; break;
    case  67 : pfnX = ut_ATime_General; break;
    case  70 : pfnX = ut_ANamePointerPair_General; break;
    case  75 : pfnX = ut_AXmlAttributes_General; break;
    case  80 : pfnX = ut_AXmlElement_General; break;
    case  82 : pfnX = ut_AXmlDocument_General; break;
    case 100 : pfnX = ut_AUrl_General; break;
    case 105 : pfnX = ut_ACookie_General; break;
    case 110 : pfnX = ut_ABasePtrQueue_General; break;
    case 200 : pfnX = ut_Macros; break;
    default:
      pfnX = NULL;
  }

  int iError = 0;
  if (pfnX)
  {
    std::cerr << "\r\n___Executing test [" << iTestNumber << "]___";
    try
    {
      iError = pfnX();
    }
    catch(AException &eX)
    {
      std::cerr << "AException caught: " << eX.what() << std::endl;
      iError = 1;
    }
#ifdef __WINDOWS__
    catch(std::exception &eX)
    {
      std::cerr << "exception caught: " << eX.what() << std::endl;
      iError = 1;
    }
#endif
    catch(const char *pccX)
    {
      std::cerr << "(const char *) caught: " << pccX << std::endl;
      iError = 1;
    }
    catch(...)
    {
      std::cerr << "Unhandled exption caught." << std::endl;
      iError = 1;
    }

    if (iError)
    {
      std::cerr << std::endl << "!!!!!! Error occured for " << iTestNumber << " err=" << iError << std::endl;
    }
    std::cerr << "\r\n_______________________________________________" << std::endl;
  }
  else
  {
    if (!boolAllTests)
      std::cerr << "Test case ID not implemented" << std::endl;
  }

  return iError;
}

int main(int iArgCount, char **ppcArgValue)
{
DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(true);

  ATimer timer;
  if (iArgCount < 2)
  {
    display();
    return -0x1;
  }

  std::cout << "\r\nExecuting Unit Tests...\r\n\r\n" << std::endl;

  int iErrorTotal = 0;
  timer.start();
  for (int iX = 0x1; iX < iArgCount; ++iX)
  {
    //a_Parse each parameter
    u4 executeCount = 1;
    VECTOR_AString params;
    AString str(ppcArgValue[iX]);
    str.split(params, ':');
    assert(params.size() > 0);

    int iTestCase = params.at(0).toU4();;
    if (params.size() > 1)
      executeCount = params.at(1).toU4();

    for (u4 uE = 0; uE < executeCount; ++uE)
      if (iTestCase > 0)
      {
        iErrorTotal += executeTest(iTestCase);
      }
      else
      {
        //a_Execute all tests in sequence
        boolAllTests = true;
        for (int iT = 0x0; iT < MAX_TESTCASE; ++iT)
          iErrorTotal += executeTest(iT + 0x1);
      }
  }
  timer.stop();
  std::cerr << "\n\nDuration of the entire test: " << std::dec << timer.getInterval() << "ms" << std::endl;
  std::cerr << "Total errors detected: " << iErrorTotal << std::endl;

DEBUG_MEMORY_LEAK_ANALYSIS_END();
  return 0x1;
}
