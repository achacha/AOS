#include "AXmlDocument.hpp"
#include "AFile_Physical.hpp"
#include "AFile_AString.hpp"
#include "ARope.hpp"
#include "AFile_IOStream.hpp"

void testConfigFile()
{
  AFile_Physical configfile("seller_view.xml");
  configfile.open();

  AXmlDocument doc("root");
  doc.fromAFile(configfile);

#ifdef __DEBUG_DUMP__
  doc.debugDump();
#endif

  ARope rope;
  doc.emit(rope, 0);

  std::cout << rope.toAString() << std::endl;
}

void testFromElement(AString& strXml)
{
/*
<task>
  <execute>
    <DBLoad name='session' name='seller'>
      <table><![CDATA[users]]>
      </table>
      <where><![CDATA[id=$INPUT.sessionid]]>
      </where>
    </DBLoad>
    <Xslt>
      <filename><![CDATA[selling/display_seller.xsl]]>
      </filename>
    </Xslt>
    <execute>
      <DBLoad name='seller_items' name='seller_prefs'>
        <table><![CDATA[user_prefs]]>
        </table>
        <where><![CDATA[id=$seller.id]]>
        </where>
      </DBLoad>
    </execute>
  </execute>
</task>
*/
  AXmlElement element;
  element.fromAFile(AFile_AString(strXml));

  ARope rope;
  element.emit(rope, 0);
  std::cout << rope.toAString() << std::endl;
}

void testFinder(AString& strXml)
{
  AXmlDocument doc("root");
  doc.fromAFile(AFile_AString(strXml));
  ARope rope;
  doc.emit(rope);

  std::cout << rope << std::endl;


#ifdef __DEBUG_DUMP__
  doc.debugDump();
#endif
}

void testDocumentBuilding()
{
  AXmlDocument doc("root");
  doc.addInstruction(AXmlInstruction::XML_HEADER)
    .addAttribute("version", "1.0")
    .addAttribute("encoding", "UTF-8");
  doc.addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute("href", "http://127.0.0.1:12346/admin_list_object.xsl")
    .addAttribute("type", "text/xsl");

  doc.useRoot().addElement("title").addAttribute("name", "title_0");
  
  AXmlElement& e = doc.useRoot().addElement("data");
  e.addData(ASWNL("Data of the data")).addAttribute("format", "0");

  //AFile_IOStream file;
  AFile_Physical file("q:/AXmlDocumentTest.xml", "w");
  doc.emit(file, 0);
}

void testDocumentParsing()
{
  AString strXml("\
<?xml version='1.0' encoding='UTF-8'?>\
<?xml-stylesheet href='/admin_list_object.xsl' type='text/xsl' ?>\
<task>\
  <execute>\
    <DBLoad name='session'>\
      <table>sessions</table>\
      <where>id=$INPUT.sessionid</where>\
    </DBLoad>\
    <!-- comment for seller -->\
    <DBLoad name='seller'>\
      <table>users</table>\
      <where>id=$INPUT.sessionid</where>\
    </DBLoad>\
    <execute>\
      <DBLoad name='seller_items'>\
        <table>items</table>\
        <where>seller_id=$seller.id</where>\
      </DBLoad>\
      <DBLoad name='seller_prefs'>\
        <!-- comment for user_prefs table -->\
        <table>user_prefs</table>\
        <where>id=$seller.id</where>\
      </DBLoad>\
    </execute>\
    <Xslt>\
      <filename>selling/display_seller.xsl</filename>\
    </Xslt>\
  </execute>\
</task>");

  AXmlDocument doc("root");
  AFile_AString strfile(strXml);
  doc.fromAFile(strfile);

#ifdef __DEBUG_DUMP__
  doc.debugDump();
#endif

  ARope rope;
  doc.emit(rope, 0);
  std::cout << rope.toAString() << std::endl;
}

void testFromFile()
{
  AXmlDocument docOriginal("root");
  AFile_AString strfile;
  docOriginal.toAFile(strfile);

  AXmlDocument docNew(strfile);

  AString strOriginal, strNew;
  docOriginal.emit(strOriginal);
  docNew.emit(strNew);

  std::cout << "-----START: original-----" << std::endl;
  std::cout << strOriginal << std::endl;
  std::cout << "-----END:   original-----" << std::endl;
  
  std::cout << "-----START: new----------" << std::endl;
  std::cout << strNew << std::endl;
  std::cout << "-----END:   new----------" << std::endl;
}

int main()
{
/*  AString strXml("\
<command name='DisplayInputMultiPart'>\
	<input name='multipart/form-data'/>\
	<module>PublishInput</module>\
	<output name='xml'/>\
</command>");
*/
    
  try
  {
  //  testConfigFile();
  //  testFromElement(strXml);
  //  testFinder(strXml);
  //  testDocumentBuilding();
  //  testDocumentParsing();
    testFromFile();
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return 0;
}