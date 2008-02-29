#include <AEventVisitor.hpp>
#include <ARope.hpp>
#include <AXmlDocument.hpp>
#include <AFile_IOStream.hpp>

void testCheckpoints()
{
  AEventVisitor visitor(ASWNL("My Visitor's Name"), AEventVisitor::EL_WARN);
  visitor.startEvent(ASWNL("main: Starting the test"));
  visitor.startEvent(ASWNL("main: Adding first line"));
  visitor.startEvent(ASWNL("main: Replacing the initial line"), AEventVisitor::EL_WARN);
  visitor.startEvent(ASWNL("main: Setting second line"));
  visitor.startEvent(ASWNL("main: Debug event state"), AEventVisitor::EL_DEBUG);
  visitor.startEvent(ASWNL("main: Adding first error"), AEventVisitor::EL_ERROR);
  visitor.startEvent(ASWNL("main: Oops, another error"), AEventVisitor::EL_ERROR);
  visitor.startEvent(ASWNL("main: Last event"));
  visitor.stopEvent();

  std::cout << "----Textual current----" << std::endl;
  ARope rope;
  visitor.emit(rope);
  std::cout << rope << std::endl;
  
  std::cout << "----Textual debug----" << std::endl;
  rope.clear();
  visitor.emit(rope, AEventVisitor::EL_DEBUG);
  std::cout << rope << std::endl;

  std::cout << "----Textual only errors----" << std::endl;
  rope.clear();
  visitor.emit(rope, AEventVisitor::EL_ERROR);
  std::cout << rope << std::endl;

  std::cout << "----Textual only events and errors----" << std::endl;
  rope.clear();
  visitor.emit(rope, AEventVisitor::EL_EVENT);
  std::cout << rope << std::endl;

  std::cout << "----XML----" << std::endl;
  AFile_IOStream iii;
  AXmlDocument doc("AEventVisitor");
  visitor.emitXml(doc.useRoot());
  doc.emit(iii, 0);
  std::cout << std::endl;
}

int main()
{
  testCheckpoints();

  return 0x0;
}
