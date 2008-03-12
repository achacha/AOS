#include "AXmlAttributes.hpp"

void testOrder()
{
  AXmlAttributes attr;
  attr.insert("Z1");
  attr.insert("B2");
  attr.insert("T3");
  attr.insert("A4");

  AString str;
  std::cout << attr << std::endl;
}

void testSimpleParse()
{
  AXmlAttributes attr;

  attr.parse("name='1' value=23982 time=16:20:00");

  attr.debugDump();
}

void testMultiple()
{
  AXmlAttributes attr;

  attr.parse("gear='broken' gear='rusty' gear='shiny' sproket='green' sproket='blue'");
  attr.insert("gear", "dull");

  attr.debugDump();
}

void testCopy1()
{
  AXmlAttributes attrs;
  attrs.parse("gear1='broken' gear2='rusty' gear3='shiny' sproket1='green' sproket2='blue'");
  attrs.insert("gear4", "dull");

  AXmlAttributes attrsCopy(attrs);

  attrs.debugDump();
  attrsCopy.debugDump();
}

void testCopy2()
{
  AXmlAttributes attrs;
  attrs.parse("gear1='broken' gear2='rusty' gear3='shiny' sproket1='green' sproket2='blue'");
  attrs.insert("gear1", "dull");

  AXmlAttributes attrsCopy;
  attrsCopy.insert("this", "should be overwritten");

  attrsCopy = attrs;
  attrs.debugDump();
  attrsCopy.debugDump();
}

int main()
{
  testOrder();
  //testSimple();
  //testMultiple();
  //testCopy1();
  //testCopy2();

  return 0x0;
}
