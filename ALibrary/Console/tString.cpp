#include <iostream.h>
#include <AString.hpp>

void testString()
{
  AString strTest(1024ul, 256ul);
  
  unsigned long dwEnd = 0x0,
                dwStart = ::GetTickCount();

  int iT;

  //a_Construction
  cout << "#Construction:" << flush;
  char sBuffer[3];
  sBuffer[0x0] = '0';
  sBuffer[0x1] = '\x0';
  sBuffer[0x2] = '\x0';
  for (iT = 0x0; iT < 32767; ++iT)
  {
    sBuffer[0x0] = char(iT % 256);
    AString str0,
            str1("01234567890abcdefghijklmnopqrstuvwxyz"),
            str2(sBuffer),
            str3(str2);
  }
  dwEnd = ::GetTickCount();
  cout << "  duration=" << dwEnd - dwStart << " ms" << endl;
  dwStart = dwEnd;


  //a_Append chars
  cout << "#Append::char:" << flush;
  for (iT = 0x0; iT < 65535; ++iT)
    strTest += '_';
  cout << "  length=" << strTest.length() << flush;
  dwEnd = ::GetTickCount();
  cout << "  duration=" << dwEnd - dwStart << " ms" << endl;
  dwStart = dwEnd;

  //a_Append a string
  cout << "#Append::string:" << flush;
  for (iT = 0x0; iT < 32767; ++iT)
    strTest += "0000000000";
  cout << "  length=" << strTest.length() << flush;
  dwEnd = ::GetTickCount();
  cout << "  duration=" << dwEnd - dwStart << " ms" << endl;
  dwStart = dwEnd;

  //a_Access
  cout << "#Access::sequential:" << flush;
  char cX;
  for (iT = 0x0; iT < strTest.length(); ++iT)
  {
    cX = strTest.at(iT);
//    cerr << cX;
//    if (cX != '0' && cX != '_')
//      cerr << "!" << flush;
  }
  dwEnd = ::GetTickCount();
  cout << "  duration=" << dwEnd - dwStart << " ms" << endl;
  dwStart = dwEnd;

  //a_find
  cout << "#Access::find:" << flush;
  strTest += '!';
  for (iT = 0x0; iT < 32767; ++iT)
  {
    strTest.find('_');
    strTest.find('0');
    strTest.find('!');
  }
  dwEnd = ::GetTickCount();
  cout << "  duration=" << dwEnd - dwStart << " ms" << endl;
  dwStart = dwEnd;
}

void testAccessOfNull()
{
  //a_Should allow to print the entire string until NULL
  AString str("Ack");
  const char* p = str.c_str();
  for (;*p;++p)
  {
    cout << *p << flush;
  }

  int iIndex = 0x0;
  char c = str.at(iIndex);
  do
  {
    cout << c << flush;
    c = str.at(++iIndex);
  } while (c);
}

#undef main
int main()
{
  //testString();
  testAccessOfNull();

  return 0x0;
}