/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AXmlDocument.hpp"

int ut_AXmlDocument_General()
{
  std::cerr << "ut_AXmlDocument_General" << std::endl;

  int iRet = 0x0;
  
  AString str(8188, 1024);
  AXmlDocument doc("root");
  doc.useRoot().addElement("foo").addData("bar");
  doc.clear();
  doc.emit(str);
  ASSERT_UNIT_TEST(str.equals("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<root/>"), "Clear", "0", iRet);

  str.clear();
  doc.clear();
  doc.useRoot().useName().clear();
  doc.emit(str);
  ASSERT_UNIT_TEST(str.isEmpty(), "Clear", "1", iRet);

  return iRet;
}