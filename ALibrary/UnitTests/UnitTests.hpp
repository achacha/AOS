/*
Written by Alex Chachanashvili

$Id$
*/

//a_Predclare all the unit tests

int ut_AString_General();                 // 1
int ut_AString_Compare();                 // 2
int ut_AString_Access();                  // 3
int ut_AString_Search();                  // 4
int ut_AString_Utility();                 // 5
int ut_AString_Modify();                  // 6
int ut_AStringHashMap_General();          // 7
int ut_ARope_General();                   // 8
int ut_ARopeDeque_General();              // 9
int ut_AException_General();              // 10
int ut_ATextConverter_General();          // 12
int ut_ATextOdometer_General();           // 13
int ut_AFilename_General();               // 14
int ut_AFileIOStream_General();           // 15
int ut_AFileIOStream_Manip();             // 16
int ut_AFileAString_General();            // 17
int ut_AFileSystem_General();             // 19
int ut_AINIProfile_General();							// 20
int ut_ARandomNumberGenerator_General();  // 25
int ut_ATextGenerator_General();          // 26
int ut_AMD5Digest_General();              // 27
int ut_AChecksum_General();               // 28
int ut_ASHA1Digest_General();             // 29
int ut_ANumber_General();                 // 35
int ut_ANumber_Internal();                // 36
int ut_ANumber_Functionality();           // 37
int ut_ANumber_Unique();                  // 38
int ut_ANameValuePair_General();          // 39
int ut_AFragmentString_General();         // 40
int ut_ABitArray_General();               // 41
int ut_AObject_General();                 // 50
int ut_AFile_Socket_General();            // 52
int ut_AFixedSizeCounter_General();       // 60
int ut_ATimer_General();                  // 65
int ut_ATime_General();                   // 67
int ut_ANamePointerPair_General();        // 70
int ut_AXmlAttributes_General();          // 75
int ut_AXmlElement_General();             // 80
int ut_AXmlDocument_General();            // 82
int ut_AUrl_General();                    // 100
int ut_ACookie_General();                 // 105
int ut_Macros();                          // 200

//a_Max testcase ID
#define MAX_TESTCASE 200

//a_Define a type for the callbacks test functions
typedef int(_fnTEST)();

//a_Optional flags
//#define __UT_ANUMBER_INTERNAL__ 1   //a_Tests ANumber internal routines
