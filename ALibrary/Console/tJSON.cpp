#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AFile_IOStream.hpp"

void test0()
{
  AXmlElement root("root");
  root.addElement("swiper").addAttribute("id", "1").addData(ASWNL("no swiping"));
  root.addElement("swiper").addAttribute("id", "2").addData(ASWNL("No swiping"));
  root.addElement("swiper").addAttribute("id", "3").addData(ASWNL("No Swiping"));

  AFile_IOStream io;
  root.emitJson(io, 0);
}

void test1()
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
  doc.emitJson(io, 0);
  //doc.emitJSON(io);

  //std::cout << str << std::endl;

}

int main()
{
  test0();
  //test1();

  return 0;
}