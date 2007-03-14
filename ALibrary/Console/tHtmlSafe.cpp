#include <ATextConverter.hpp>

#undef main
int main()
{
  AString str("!<HTML>&...");
  cout << str.c_str() << endl; 
  cout << ATextConverter::makeHtmlSafe(str).c_str() << endl; 
  
  str.assign("<><HTML>&...&");
  cout << str.c_str() << endl; 
  cout << ATextConverter::makeHtmlSafe(str).c_str() << endl; 

  return 0x0;

}
