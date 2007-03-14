#include <AEventVisitor.hpp>
#include <ARope.hpp>
#include <AXmlDocument.hpp>
#include <AFile_IOStream.hpp>

void testCheckpoints()
{
  AEventVisitor visitor("My Visitor's Name");
  visitor.set(ASWNL("main: Starting the test"));
  visitor.set(ASWNL("main: Adding first line"));
  visitor.set(ASWNL("main: Replacing the initial line"));
  visitor.set(ASWNL("main: Setting second line"));
  visitor.set(ASWNL("main: Another event state"));
  visitor.set(ASWNL("main: Adding first error"), true);
  visitor.set(ASWNL("main: Oops, another error"), true);
  visitor.set(ASWNL("main: Last event"));
  visitor.reset();

  std::cout << "----Textual----" << std::endl;
  ARope rope;
  visitor.emit(rope);
  std::cout << rope << std::endl;
  
  std::cout << "----XML----" << std::endl;
  AFile_IOStream iii;
  AXmlDocument doc("AEventVisitor");
  visitor.emit(doc.useRoot());
  doc.emit(iii, 0);
  std::cout << std::endl;
}

int main()
{
  testCheckpoints();

  return 0x0;
}
