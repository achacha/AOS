/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "AXmlData.hpp"

void testGetSet(int& iRet)
{
  AXmlElement elem("root");
  elem.addAttribute("a1", "foo");
  elem.addAttribute("a2", "foo");
  elem.addAttribute("a3", "foo");
  elem.addElement("path0").addElement("path1").addElement("path2").addComment("My comment");
  AString str0(8188, 1024), str1(8188, 1024);
  elem.emit(str0);
  ASSERT_UNIT_TEST(str0.equals("<root a1=\"foo\" a2=\"foo\" a3=\"foo\"><path0><path1><path2><!--My comment--></path2></path1></path0></root>"), "add_emit", "0", iRet);
  
  elem.clear();
  str0.clear();
  elem.emit(str0);
  ASSERT_UNIT_TEST(str0.equals("<root/>"), "clear", "0", iRet);
}

void testAddAndSerialize(int& iRet)
{
  AXmlElement elem("root");
  AXmlElement& path1 = elem.addElement(ASWNL("path0")).addElement(ASWNL("path1"));
  AXmlElement& path2 = elem.findElement(ASWNL("path0"))->addElement(ASWNL("path2"));
  path1.addElement(ASWNL("obj0")).addData(ASWNL("value0"));
  path2.addElement(ASWNL("obj1")).addData(ASWNL("value1"));
  path1.addElement(ASWNL("obj2")).addData(ASWNL("value2"));
  elem.addElement(ASWNL("path0/path1/obj3"), ASWNL("value3"), AXmlElement::ENC_NONE, true);  //insert

  AString str0(8188, 1024), str1(8188, 1024);
  str1.assign("\
<root>\r\n\
  <path0>\r\n\
    <path1>\r\n\
      <obj0>value0</obj0>\r\n\
      <obj2>value2</obj2>\r\n\
      <obj3>value3</obj3>\r\n\
    </path1>\r\n\
    <path2>\r\n\
      <obj1>value1</obj1>\r\n\
    </path2>\r\n\
  </path0>\r\n\
</root>");
  elem.emit(str0, 0);
  //std::cout << str0 << std::endl;
  ASSERT_UNIT_TEST(str0.equals(str1), "add_emit", "1", iRet);

  AXmlElement *p;
  if (p = elem.findElement("/root/path0/path1/"))
  {
    p->useAttributes().insert("attrib0", "0");
    p->useAttributes().insert("attrib1", "0");
    p->useAttributes().insert("attrib2", "0");
    p->addContent(new AXmlElement(ASWNL("foo")));
    p->addContent(new AXmlData(ASWNL("more data")));
    p->addComment("This is the comment");

    str0.clear();
    elem.emit(str0, 0);
//    std::cout << str0 << std::endl;
    ASSERT_UNIT_TEST(str0.equals("<root>\r\n\
  <path0>\r\n\
    <path1 attrib0=\"0\" attrib1=\"0\" attrib2=\"0\">\r\n\
      <obj0>value0</obj0>\r\n\
      <obj2>value2</obj2>\r\n\
      <obj3>value3</obj3>\r\n\
      <foo/>more data<!--This is the comment-->\r\n\
    </path1>\r\n\
    <path2>\r\n\
      <obj1>value1</obj1>\r\n\
    </path2>\r\n\
  </path0>\r\n\
</root>"), "add_emit", "2", iRet);
  }
  else
    ASSERT_UNIT_TEST(p, "find", "0", iRet);

  AFile_AString strfile;
  elem.toAFile(strfile);
//  std::cout << strfile.useAString() << std::endl;
  ASSERT_UNIT_TEST(strfile.useAString().equals("\
<root><path0><path1 attrib0=\"0\" attrib1=\"0\" attrib2=\"0\"><obj0>value0</obj0>\
<obj2>value2</obj2><obj3>value3</obj3><foo/>more data<!--This is the comment-->\
</path1><path2><obj1>value1</obj1></path2></path0></root>"), "toAFile", "", iRet);

  AXmlElement restored;
  restored.fromAFile(strfile);
  str1.clear();
  restored.emit(str1, 0);
  //std::cout << str0 << std::endl << str1 << std::endl;

  ASSERT_UNIT_TEST(str0.equals(str1), "Serialization", "", iRet);
}

void testXmlElementAccess(int& iRet)
{
  AXmlElement root("root");
  root.addElement("base");
  AXmlElement& config = root.addElement("config");
  config.addElement("red").addData("100");
  config.addElement("blue").addAttribute("id", "3").addData("endless");
  config.addElement("green");

  ASSERT_UNIT_TEST(root.getU4("/root/base", 13) == 13, "Access", "0", iRet);
  ASSERT_UNIT_TEST(root.getU4("/root/config/blue@id", 13) == 3, "Access", "1", iRet);
  ASSERT_UNIT_TEST(root.getU4("/root/red", 13) == 100, "Access", "2", iRet);
  ASSERT_UNIT_TEST(root.getU4("/root/green", 13) == 13, "Access", "3", iRet);

}


int ut_AXmlElement_General()
{
  std::cerr << "ut_AXmlElement_General" << std::endl;

  int iRet = 0x0;
  
  testGetSet(iRet);
  NEWLINE_UNIT_TEST();
  testAddAndSerialize(iRet);
  NEWLINE_UNIT_TEST();
  testXmlElementAccess(iRet);

  return iRet;
}