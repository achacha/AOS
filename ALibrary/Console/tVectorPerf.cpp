#include <iostream.h>
#include <ANameValuePair.hpp>
#include <AVectorOfABase.hpp>
#include <ATimer.hpp>

void findAndPrint(const AVectorOfABase& v, const AString& str)
{
  for (int i = 0x0; i < v.getSize(); ++i)
    if ( ((ANameValuePair*)v.getAt(i))->isName(str) )
      cerr << ((ANameValuePair*)v.getAt(i))->toString().c_str() << endl;
    else
      cerr << str.c_str() << ": Not found" << endl;
}

int main()
{
  AVectorOfABase v;
  ANameValuePair *p;
  AString str(1024, 16);
  for (int iX = 0x0; iX < 10; ++iX)
  {
    str = AString::fromInt(iX);
    p = new ANameValuePair(str, AString("_value") + AString::fromInt(iX));
    v.pushFront(p);
  }

  
  findAndPrint(v, "4");
  findAndPrint(v, "0");
  findAndPrint(v, "7");
  findAndPrint(v, "9");

  findAndPrint(v, "-1");
  findAndPrint(v, "10");

  return 0x0;
}