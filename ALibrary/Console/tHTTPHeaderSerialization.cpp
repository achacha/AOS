#include "AString.hpp"
#include "AFile_AString.hpp"
#include "AFile_IOStream.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AContentType_Form.hpp"
#include "AContentType_TextHtml.hpp"

void testRequestHeader()
{
  AString strRequestHeader("POST /cgi-bin/variables.exe HTTP/1.1\r\n\
Host: localhost:13666\r\n\
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.7.8) Gecko/20050511 Firefox/1.0.4\r\n\
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n\
Accept-Language: en-us,en;q=0.5\r\n\
Accept-Encoding: gzip,deflate\r\n\
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n\
Keep-Alive: 300\r\n\
Connection: keep-alive\r\n\
Referer: http://localhost:13666/cgi-bin/variables.exe\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Content-Length: 15\r\n\
\r\n\
%21EnvName=path\r\n");
  
  //a_Read HTTP request header
  AFile_AString strfile(strRequestHeader);
  AHTTPRequestHeader header;
  header.fromAFile(strfile);
  
  //a_Read form data
  AContentType_Form content;
  content.parseHTTPHeader(header);
  content.fromAFile(strfile);

  //a_Display header and content
  AFile_IOStream iosfile;
  header.toAFile(iosfile);
  content.toAFile(iosfile);
}

void testResponseHeader()
{
  AString strResponseHeader("HTTP/1.1 200 OK\r\n\
Server: ALibrary Simple Server 1.0\r\n\
Date: Tue, 14 Jun 2005 00:02:10 GMT\r\n\
Connection: close\r\n\
Content-Type: text/html\r\n\
\r\n\
<html><body>HelloWorld!</body></html>");

  //a_Parse header
  AFile_AString strfile(strResponseHeader);
  AHTTPResponseHeader header;
  header.fromAFile(strfile);

  //a_Read content
  AContentType_TextHtml content;
  content.parseHTTPHeader(header);
  content.fromAFile(strfile);

  //a_Display header and content
  AFile_IOStream iosfile;
  header.toAFile(iosfile);
  content.toAFile(iosfile);
}

int main()
{
//  testRequestHeader();
  testResponseHeader();

  return 0;
}
