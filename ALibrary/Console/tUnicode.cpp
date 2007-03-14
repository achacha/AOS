#include <iostream.h>
#include <AString.hpp>
#include <ATextConverter.hpp>

void testFromTo()
{
  AString str("%E6%AD%BB%E7%8B%97");
  str = ATextConverter::decodeURL(str);
  wchar_t wc00, wc01, wc10;
  u4 uPos = 0x0;

  wc01 = str.toUnicode();
  wc00 = str.toUnicode(uPos);

  if (wc00 != wc01)
  {
    cerr << "ERROR: wc00 != wc01" << endl;
  }

  wc10 = str.toUnicode(uPos);
  if (uPos != AString::npos)
  {
    cerr << "ERROR: EOS not detected." << endl;
  }

  AString str1 = AString::fromUnicode(wc00) + AString::fromUnicode(wc10);
  if (str1.compare(str))
  {
    cerr << "ERROR: fromUnicode did not covert correctly" << endl;
  }
}

void testNonUnicode()
{
  AString str("I am not unicode!");
  wchar_t wc = str.toUnicode();
  if (wc != (int)'I')
  {
    cerr << "ERROR: Conversion failed" << endl;
  }
}

void unicodeCGI()
{
  const wchar_t ws[] = { 0x041B, 0x0410, 0x0414, 0x0410, 0x0000 };
  AString str(ws);
  
  cout << "Content-Type: text/html" << endl;
  cout << endl;
  cout << "<HTML>" << endl;
  cout << "<HEAD>" << endl;
  cout << "  <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; CHARSET=UTF-8\">" << endl;
  cout << "  <TITLE>Unicode UTF-8 CGI</TITLE>" << endl;
  cout << "</HEAD>" << endl;
  cout << "<BODY>" << endl;
  
  cout << "<B LANG=EN>Hello</B> " << endl;
  cout << "<B LANG=RU>" << endl;
  cout << str.c_str() << endl;
  cout << "</B><BR><HR><BR>" << endl;

  wchar_t *pwcX = str.getDoubleByte();

  if (wcscmp(ws, pwcX))
    cout << "<BIG STYLE=\"fontSize='18pt'; color='RED'\">NOT EQUAL!</BIG>" << endl;
  else
    cerr << "<BIG STYLE=\"fontSize='18pt'; color='BLUE'\">EQUAL!</BIG>" << endl;

  cout << "</BODY></HTML>" << endl;
}

int main()
{
//  unicodeCGI();
//  testFromTo();
//  testNonUnicode();

  return 0x0;
}
