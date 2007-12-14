
#include <AContentType_TextHtml.hpp>

void testHTMLparse()
{
  AContentType_TextHtml html;

  html.useData().append("<HTML><BODY TEXT=#FFFFFF><H1>HelloWorld</H1>\
Another text line\
<FORM METHOD=\"POST\" ACTION=\"execute.cgi\">\
<INPUT TYPE=\"TEXT\" SIZE=\"20\" NAME=\"myname\">\
<INPUT TYPE=\"SUBMIT\" NAME=\"_submit\"></FORM>\
<A HREF=\"http://www.somewhere.com/page.html\">here</A>\
</BODY></HTML>");
  AUrl urlBase("http://www.myserver.com:8000/mypath/index.html");
  
  html.parseAll(urlBase);
  try
  {
    html.lint();
  }
  catch(AException& e)
  {
    std::cerr << e << std::endl;
  }
  html.debugDump();

  std::cout << "---------------------raw--------------------------" << std::endl;
  std::cout << html << std::endl;

  std::cout << "------------------formatted-----------------------" << std::endl;
  html.setFormatted();
  std::cout << html << std::endl;

  int iX, iSize = html.getFormCount();
  for (iX = 0x0; iX < iSize; ++iX)
  {
    std::cout << html.getFormObserver(iX).getSubmissionUrl() << std::endl;
  }

  iSize = html.getHrefCount();
  for (iX = 0x0; iX < iSize; ++iX)
  {
    std::cout << html.getHrefObserver(iX).getSubmissionUrl() << std::endl;
  }
}

void testHTMLparseErrors()
{
  AContentType_TextHtml html;

  html.useData().append("<HTML></PRE>");
  AUrl urlBase("http://www.myserver.com:8000/mypath/index.html");
  
  html.setExtendedErrorLogging();
  html.parse();

  std::cout << "---------------------raw--------------------------" << std::endl;
  std::cout << html << std::endl;

  std::cout << "------------------formatted-----------------------" << std::endl;
  html.setFormatted();
  std::cout << html << std::endl;

}

void testHTMLparseScript()
{
  AContentType_TextHtml html;

  html.useData().append("<HTML>\
<SCRIPT LANGUAGE=\"JavaScript\">\
<!-- \
open(\"http://127.0.0.1/tWAXTest.html\")\
//-->\
</SCRIPT>\
\
<SCRIPT LANGUAGE=\"JavaScript 1.2\">\
\
<!-- \
function opencrap()\
{\
  window.open(\"http://127.0.0.1/tWAXTest.html\")\
}\
//-->\
</SCRIPT>\
\
<BODY ONLOAD=\"javascript:opencrap();\"> Hello World \
</BODY></HTML>");
  AUrl urlBase("http://www.myserver.com:8000/mypath/index.html");
  
  html.parse();
  try
  {
    html.lint();
  }
  catch(AException& e)
  {
    std::cerr << e << std::endl;
  }

  AElement_HTML *pe = NULL;
  AString str;
  while (pe = html.findType("SCRIPT", pe))
  {
    std::cout << "===============================" << std::endl;
    pe->recurseToString(str);
    
//    if (str.findNoCase("ONLOAD"))
    std::cout << str << std::endl;
    str.clear();
  }

  //a_Remove the ONLOAD tag in the BODY
  if (pe = html.findType("BODY", NULL))
  {
    std::cout << "===============================" << std::endl;
    if (pe->isName("ONLOAD"))
    {
      std::cout << "Found ONLOAD, removing" << std::endl;
      pe->removeName("ONLOAD");
    }
  }
}

int main()
{
  testHTMLparse();
  //testHTMLparseErrors();
  testHTMLparseScript();
  
  return 0x0;
}