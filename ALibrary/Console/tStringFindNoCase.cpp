
#include <AString.hpp>

void testFindNoCase()
{
  //            0         1         2         3         4         5
  //            012345678901234567890123456789012345678901234567890123456789
  AString str0("<BODY ONLOAD=javascript:foo('00') BGCOLOR=#000000>");
  u4 uPos = str0.findNoCase("onLoad");
  cout << "onLoad = " << uPos << endl;

  uPos = str0.findNoCase('b');
  cout << "b = " << uPos << endl;

  uPos = str0.findNoCase("xxx");  //a_Not found
  cout << "xxx = " << uPos << endl;

  uPos = str0.findNoCase('x');  //a_Not found
  cout << "xxx = " << uPos << endl;

  uPos = str0.rfindNoCase("00");
  cout << "00 = " << uPos << endl;

  uPos = str0.rfindNoCase('b');
  cout << "b = " << uPos << endl;

  uPos = str0.rfindNoCase("xxx"); //a_Not found
  cout << "b = " << uPos << endl;

  uPos = str0.rfindNoCase('x'); //a_Not found
  cout << "b = " << uPos << endl;
}

#undef main
int main()
{
  testFindNoCase();

  return 0x0;
}