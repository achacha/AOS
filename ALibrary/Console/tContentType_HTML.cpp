
#include <AContentType_TextHtml.hpp>

void testHTMLparse()
{
  AContentType_TextHtml html;

  AString strHtml("<HTML><BODY TEXT=#FFFFFF><H1>HelloWorld</H1>\
Another text line\
<FORM METHOD=\"POST\" ACTION=\"execute.cgi\">\
<INPUT TYPE=\"TEXT\" SIZE=\"20\" NAME=\"myname\">\
<INPUT TYPE=\"SUBMIT\" NAME=\"_submit\"></FORM>\
<A HREF=\"http://www.somewhere.com/page.html\">here</A>\
</BODY></HTML>");
  AUrl urlBase("http://www.myserver.com:8000/mypath/index.html");
  
  html.addData(strHtml);
  html.parseAll(urlBase);

  cout << "---------------------raw--------------------------" << endl;
  cout << html.toString() << endl;

  cout << "------------------formatted-----------------------" << endl;
  html.setFormatted();
  cout << html.toString() << endl;

  int iX, iSize = html.getFormCount();
  for (iX = 0x0; iX < iSize; ++iX)
  {
    cout << html.getFormObserver(iX).getSubmissionUrl().toString().c_str() << endl;
  }

  iSize = html.getHrefCount();
  for (iX = 0x0; iX < iSize; ++iX)
  {
    cout << html.getHrefObserver(iX).getSubmissionUrl().toString().c_str() << endl;
  }
}

void testHTMLparseErrors()
{
  AEventVisitor ev;
  AContentType_TextHtml html(&ev);

  AString strHtml("<HTML></PRE>");
  AUrl urlBase("http://www.myserver.com:8000/mypath/index.html");
  
  html.addData(strHtml);
  ev.setTimingMode();
  ev.setVerboseMode();
  html.setExtendedErrorLogging();
  html.parse();

  cout << "---------------------raw--------------------------" << endl;
  cout << html.toString() << endl;

  cout << "------------------formatted-----------------------" << endl;
  html.setFormatted();
  cout << html.toString() << endl;

  cout << "----------------event visitor---------------------" << endl;
  ev.setDisplayMode(AEventVisitor::All);
  ev.toOStream(cout);
  cout << endl;
}

void testHTMLparseScript()
{
  AEventVisitor ev;
  AContentType_TextHtml html(&ev);

  AString strHtml("<HTML>\
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
  
  html.addData(strHtml);
  html.parse();
  html.lint();

  AElement_HTML *pe = NULL;
  AString str;
  while (pe = html.findType("SCRIPT", pe))
  {
    cout << "===============================" << endl;
    pe->recurseToString(str);
    
//    if (str.findNoCase("ONLOAD"))
    cout << str.c_str() << endl;
    str.clear();
  }

  //a_Remove the ONLOAD tag in the BODY
  if (pe = html.findType("BODY", NULL))
  {
    cout << "===============================" << endl;
    if (pe->isName("ONLOAD"))
    {
      cout << "Found ONLOAD, removing";
      pe->removeName("ONLOAD");
    }
  }

  cout << "\r\n\r\n----------------event visitor---------------------" << endl;
  ev.setDisplayMode(AEventVisitor::All);
  ev.toOStream(cout);
  cout << endl;
}

#undef main
int main()
{
  //testHTMLparse();
  //testHTMLparseErrors();
  testHTMLparseScript();
  
  return 0x0;
}