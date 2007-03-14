#include "AAttributes.hpp"

void testSimple()
{
  AAttributes attr;

  attr.parse("name='1' value=23982 time=16:20:00");

  attr.debugDump();
}

void testMultiple()
{
  AAttributes attr;

  attr.parse("gear='broken' gear='rusty' gear='shiny' sproket='green' sproket='blue'");
  attr.insert("gear", "dull");

  attr.debugDump();
}

void testCopy1()
{
  AAttributes attrs;
  attrs.parse("gear='broken' gear='rusty' gear='shiny' sproket='green' sproket='blue'");
  attrs.insert("gear", "dull");

  AAttributes attrsCopy(attrs);

  attrs.debugDump();
  attrsCopy.debugDump();
}

void testCopy2()
{
  AAttributes attrs;
  attrs.parse("gear='broken' gear='rusty' gear='shiny' sproket='green' sproket='blue'");
  attrs.insert("gear", "dull");

  AAttributes attrsCopy;
  attrsCopy.insert("this", "should be overwritten");

  attrsCopy = attrs;
  attrs.debugDump();
  attrsCopy.debugDump();
}

int main()
{
  //testSimple();
  //testMultiple();
  //testCopy1();
  testCopy2();

  return 0x0;
}
