#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AFile_IOStream.hpp"
int main()
{
  AFile_AString strf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<root><data><user><AResultSet><sql><![CDATA[select * from user]]></sql><row><id>1</id><password>(null)</password><username>achacha</username></row></AResultSet><AResultSet><sql><![CDATA[select * from user]]></sql><row><id>1</id><password>(null)</password><username>achacha</username></row></AResultSet></user></data>\
<total_time running=\"true\">\
  <interval freq=\"3579.545170\" unit=\"ms\">21.049</interval>\
  <start_count>3584002785716</start_count>\
  <stop_count>0</stop_count>\
</total_time></root>");
//  AFile_AString strf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
//<data>\
//  <sql><![CDATA[select * from user]]></sql>\
//  <row>\
//    <id>1</id>\
//    <password>(null)</password>\
//    <username>achacha</username>\
//  </row>\
//</data>"); 
  AXmlDocument doc(strf);
  
  AFile_IOStream io;
  //AString str;
  doc.emitJSON(io, 0);
  //doc.emitJSON(io);

  //std::cout << str << std::endl;

  return 0;
}