#include "pchUnitTests.hpp"
#include "ARope.hpp"
#include "AString.hpp"

void testRopeRandomAccess(int& iRet)
{
  ARope rope(ASW("0123456789",10), 3);
  AString str;
  size_t r = rope.access(str);
  ASSERT_UNIT_TEST(str.equals("0123456789"), "ARope::peek", "0", iRet);
  ASSERT_UNIT_TEST(r == 10, "ARope::peek", "01", iRet);

  str.clear();
  r = rope.access(str, 7, 2);
  ASSERT_UNIT_TEST(str.equals("78"), "ARope::peek", "1", iRet);
  ASSERT_UNIT_TEST(r == 2, "ARope::peek", "11", iRet);

  str.clear();
  r = rope.access(str, 7, 0);
  ASSERT_UNIT_TEST(str.isEmpty(), "ARope::peek", "2", iRet);
  ASSERT_UNIT_TEST(r == 0, "ARope::peek", "21", iRet);

  str.clear();
  r = rope.access(str, 1, 50);
  ASSERT_UNIT_TEST(str.equals("123456789"), "ARope::peek", "3", iRet);
  ASSERT_UNIT_TEST(r == 9, "ARope::peek", "31", iRet);

  str.clear();
  r = rope.access(str, 3, 3);
  ASSERT_UNIT_TEST(str.equals("345"), "ARope::peek", "4", iRet);
  ASSERT_UNIT_TEST(r == 3, "ARope::peek", "41", iRet);

  str.clear();
  r = rope.access(str, 9, 2);
  ASSERT_UNIT_TEST(str.equals("9"), "ARope::peek", "5", iRet);
  ASSERT_UNIT_TEST(r == 1, "ARope::peek", "51", iRet);
}

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
  
  rope.clear();
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

  testRopeRandomAccess(iRet);

  return iRet;
}