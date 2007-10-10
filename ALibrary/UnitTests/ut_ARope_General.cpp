#include "pchUnitTests.hpp"
#include "ARope.hpp"

int ut_ARope_General()
{
  std::cerr << "ut_ARope_General" << std::endl;

  int iRet = 0x0;

  ARope ropeEmpty;
  ASSERT_UNIT_TEST(ropeEmpty.isEmpty(), "ARope::isEmpty", "0", iRet);

  ARope rope("TextBlock0000", 8);
  rope.append("TextBlock0123456789");

  AString str;
  rope.emit(str);
  ASSERT_UNIT_TEST(!str.compare("TextBlock0000TextBlock0123456789"), "ARope::emit", "0", iRet);
  ASSERT_UNIT_TEST(!rope.toAString().compare("TextBlock0000TextBlock0123456789"), "ARope::toAString", "0", iRet);

  ARope ropeCopy(rope);
  str.clear(true);
  ropeCopy.emit(str);
  ASSERT_UNIT_TEST(!str.compare("TextBlock0000TextBlock0123456789"), "ARope::copyctor", "0", iRet);
  ASSERT_UNIT_TEST(!ropeCopy.toAString().compare("TextBlock0000TextBlock0123456789"), "ARope::copyctor", "1", iRet);
  
  rope.clear();
  ASSERT_UNIT_TEST(rope.getSize() == 0, "ARope::clear", "0", iRet);
  ASSERT_UNIT_TEST(rope.isEmpty(), "ARope::isEmpty", "1", iRet);

  rope.append('M');
  rope.append(AString("ozi"));
  rope.append("ll", 2);
  rope.append('a');
  ASSERT_UNIT_TEST(!rope.isEmpty(), "ARope::isEmpty", "2", iRet);
  ASSERT_UNIT_TEST((rope.getSize() == 7 && !rope.toAString().compare("Mozilla")), "ARope::append", "0", iRet);
  
  rope.clear(true);
  ASSERT_UNIT_TEST(rope.isEmpty(), "ARope::isEmpty", "3", iRet);
  rope.append("This is lots and lots of data to force the rope to use the m_Blocks structure");
  ASSERT_UNIT_TEST(!rope.isEmpty(), "ARope::isEmpty", "4", iRet);

  //a_Test inline concat
  rope.clear();
  ASSERT_UNIT_TEST(rope.isEmpty(), "ARope::isEmpty", "5", iRet);
  rope += ASWNL("Existing ");
  ASSERT_UNIT_TEST(!rope.isEmpty(), "ARope::isEmpty", "6", iRet);
  rope += ARope("on damnation's ")+AString("edge the priest had ")+"never known ";
  rope += AString("to witness ")+ASWNL("such a violent ")+"show of "+ARope("power overthrown");
  ASSERT_UNIT_TEST(rope.toAString().equals("Existing on damnation's edge the priest had never known to witness such a violent show of power overthrown"), "ARope::inline concat", "0", iRet);

  return iRet;
}