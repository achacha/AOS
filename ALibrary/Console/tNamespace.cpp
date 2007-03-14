#include "AObjectContainer.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"

void __populate(AObjectContainer& ns)
{
  ns.insert("/request/path/object0", new AObject<AString>(AString("something")));
  ns.insert("/request/path/object1", new AObject<AString>(AString("something else"), AXmlData::CDataDirect));
  ns.insert("/request/dir/object0", new AObject<AString>(AString("yet more!"), AXmlData::Base64));
  ns.insert("/request/dir/object1", new AObject<AString>(AString("and yet more!")));
  ns.insert("/base/object0", new AObject<AString>(AString("base object")));
}

void __display(AObjectContainer copy)  //a_Test copy as well here
{
  copy.debugDump(std::cout, 0);

  ARope rope;
  AXmlElement element("foo");
  copy.publish(element);
  element.emitXml(rope);
  std::cout << "XML publish\n" << rope << std::endl;
}

void __insert(AObjectContainer& ns)
{
  AFile_AString f("<inserted><var a=1 b=2 c=3/><mixed>foo<bar/>baz</mixed></inserted>");
  AXmlElement element;
  element.fromAFile(f);
  ARope rope;
  element.emitXml(rope);
//  element.debugDump(std::cout, 0);
//  std::cout << rope << std::endl;

  AObjectContainer nsNew;
  nsNew.fromElement(element);
  rope.clear();
  element.clear();
  nsNew.publish(element);
//  nsNew.debugDump(std::cout, 0);

  element.debugDump(std::cout, 0);
  element.emitXml(rope);
//  std::cout << rope << std::endl;

  ns.insert("/incoming/", nsNew.clone());
//  ns.debugDump(std::cout, 0);
}

void test_iterator()
{
  AFile_AString f("<inserted><var a=1 b=2 c=3/></inserted>");
  AXmlElement element;
  element.fromAFile(f);
  std::cout << "element_name=" << element.getName() << std::endl;
  AXmlElement::NodeContainer::const_iterator cit = element.getContent().begin();
  while (cit != element.getContent().end()) 
  {
    AXmlElement *p = dynamic_cast<AXmlElement *>(*cit);
    if (p)
    {
      ARope rope;
      p->emitXml(rope);
      std::cout <<  "  " << rope << std::endl;
    }
    ++cit;
  }
}

int main()
{
  AAutoPtr<AObjectContainer> pns(new AObjectContainer("root"));

  try
  {
    __populate(*pns.get());
    //test_iterator();
    __insert(*pns.get());
    //__display(*pns);
  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception caught!" << std::endl;
  }

  return 0;
}


