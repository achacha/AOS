#include <AElement_HTML.hpp>

void testHTMLparse()
{
  AElement_HTML he;

  u4 iPos = 0x0;
  AString strHtml("<HTML><BODY TEXT=#FFFFFF><H1>HelloWorld</H1>Another text line</BODY></HTML>");
  he.parse(strHtml, iPos);

  AString strR, strRI;
  he.recurseToString(strR);
  he.recurseToString(strRI, 0x0);

  std::cout << "===No Indent===" << std::endl;
  std::cout << strR.c_str() << std::endl;
  std::cout << "===With Indent===" << std::endl;
  std::cout << strRI.c_str() << std::endl;
}

void testLeadingComment()
{
  AElement_HTML he;

  u4 iPos = 0x0;
  AString strHtml("<!-------- Sample ------------><HTML><BODY TEXT=#FFFFFF><H1>HelloWorld</H1>Another text line</BODY></HTML>");
  he.parse(strHtml, iPos);

  AString strR, strRI;
  he.recurseToString(strR);

  std::cout << "===No Indent===" << std::endl;
  std::cout << strR.c_str() << std::endl;
}
  
int main()
{
  testHTMLparse();
  testLeadingComment();
  
  return 0x0;
}