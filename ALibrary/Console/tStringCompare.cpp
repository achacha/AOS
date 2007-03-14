
#include <AString.hpp>

#undef main
int main()
{
  AString str0("0123"), str1("012");

/*
  if (str0 > str1)
    cerr << str0 << ">" << str1 << endl;
  else
    cerr << str0 << "<=" << str1 << endl;
*/

  bool boolR;
  for (int iT = 0x0; iT < 1024; ++iT)
  {
    boolR = (str0 > str1);
    boolR = (str0 < str1);
    boolR = (str0 == str1);
  }

  return 0x0;
}