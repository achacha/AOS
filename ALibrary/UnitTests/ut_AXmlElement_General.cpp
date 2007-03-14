#include "pchUnitTests.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"

void testGetSet(int& iRet)
{
  AXmlElement elem("root");
  elem.addAttribute("a1", "foo");
  elem.addAttribute("a2", "foo");
  elem.addAttribute("a3", "foo");
  elem.addElement("path0").addElement("path1").addElement("path2").addComment("My comment");
  AString str0(8188, 1024), str1(8188, 1024);
  elem.emit(str0);
  ASSERT_UNIT_TEST(str0.equals("<root a1=\"foo\" a2=\"foo\" a3=\"foo\"><path0><path1><path2><!-- My comment--></path2></path1></path0></root>"), "add_emit", "0", iRet);
  
  elem.clear();
  str0.clear();
  elem.emit(str0);
  ASSERT_UNIT_TEST(str0.equals("<root/>"), "clear", "0", iRet);
}

void testAddAndSerialize(int& iRet)
{
  AXmlElement elem("root");
  elem.addElement("/path0/path1/obj0", "value0");
  elem.addElement("/path0/path2/obj1", "value1");
  elem.addElement("/path0/path1/obj2", "value2");

  AString str0(8188, 1024), str1(8188, 1024);
  elem.emit(str0, 0);
  ASSERT_UNIT_TEST(str0.equals("\
<root>\r\n\
  <path0>\r\n\
    <path1>\r\n\
      <obj0>value0</obj0>\r\n\
      <obj2>value2</obj2>\r\n\
    </path1>\r\n\
    <path2>\r\n\
      <obj1>value1</obj1>\r\n\
    </path2>\r\n\
  </path0>\r\n\
</root>"), "add_emit", "0", iRet);

  AXmlNode *p;
  if (p = elem.findNode("/root/path0/path1/"))
  {
    p->useAttributes().insert("attrib0", "0");
    p->useAttributes().insert("attrib1", "0");
    p->useAttributes().insert("attrib2", "0");
    p->addContentNode(new AXmlElement(ASWNL("foo")));
    p->addContentNode(new AXmlData(ASWNL("more data")));
    p->addComment("This is the comment");

    str0.clear();
    elem.emit(str0, 0);
    ASSERT_UNIT_TEST(str0.equals("<root>\r\n\
  <path0>\r\n\
    <path1 attrib0=\"0\" attrib1=\"0\" attrib2=\"0\">\r\n\
      <obj0>value0</obj0>\r\n\
      <obj2>value2</obj2>\r\n\
      <foo/>more data<!-- This is the comment-->\r\n\
    </path1>\r\n\
    <path2>\r\n\
      <obj1>value1</obj1>\r\n\
    </path2>\r\n\
  </path0>\r\n\
</root>"), "add_emit", "1", iRet);
  }
  else
    ASSERT_UNIT_TEST(p, "find", "0", iRet);

  AFile_AString strfile;
  elem.toAFile(strfile);
  ASSERT_UNIT_TEST(strfile.useAString().equals("\
<root><path0><path1 attrib0=\"0\" attrib1=\"0\" attrib2=\"0\"><obj0>value0</obj0><obj2>value2</obj2><foo/>more data<!-- This is the comment--></path1><path2><obj1>value1</obj1></path2></path0></root>"), "toAFile", "", iRet);

  AXmlElement restored;
  restored.fromAFile(strfile);
  restored.emit(str1, 0);
  ASSERT_UNIT_TEST(str0.equals(str1), "Serialization", "", iRet);
}

int ut_AXmlElement_General()
{
  std::cerr << "ut_AXmlElement_General" << std::endl;

  int iRet = 0x0;
  
  testGetSet(iRet);
  std::cerr << std::endl;
  testAddAndSerialize(iRet);

  return iRet;
}