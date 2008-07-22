#include "AString.hpp"
#include "AEventVisitor.hpp"
#include "templateAutoPtr.hpp"

void testNew()
{
  AString *p = new AString("foo",3);
  p->append("bar",3);
  delete p;
  p = NULL;
}

void testEventVisitor()
{
  AAutoPtr<AEventVisitor> p(new AEventVisitor(), true);
  p->startEvent(ASWNL("001"));
  p->startEvent(ASWNL("002"));
  p->startEvent(ASWNL("003"));
  p->startEvent(ASWNL("004"));
  p->addEvent(ASWNL("100"));
  p->startEvent(ASWNL("005"));
}

int main()
{
  //testNew();
  testEventVisitor();

  return 0;
}