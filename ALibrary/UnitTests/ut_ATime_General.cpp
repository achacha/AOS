#include "pchUnitTests.hpp"

#include <ATime.hpp>

int ut_ATime_General()
{
  std::cerr << "ut_ATime_General" << std::endl;

  int iRet = 0x0;

  //a_Creation
  ATime t0((time_t)0x0);
  AString str;
  t0.emitRFCtime(str);
  ASSERT_UNIT_TEST(str == AString("Thu, 01-Jan-1970 00:00:00 UT"), "ATime::emitRFCTime", "0", iRet);

  ATime tNow;
  ASSERT_UNIT_TEST(t0 < tNow, "ATime::operator<", "0", iRet);
  ASSERT_UNIT_TEST(t0 <= tNow, "ATime::operator<=", "0", iRet);
  ASSERT_UNIT_TEST(tNow > t0, "ATime::operator>", "0", iRet);
  ASSERT_UNIT_TEST(tNow >= t0, "ATime::operator>=", "0", iRet);
  ASSERT_UNIT_TEST(tNow != t0, "ATime::operator!=", "0", iRet);
  
  ATime t1(t0);
  ASSERT_UNIT_TEST(t1 == t0, "ATime::operator== and copy ctor", "0", iRet);
  ASSERT_UNIT_TEST(t1.difftime(t0) == 0.0, "ATime::difftime", "0", iRet);

  return iRet;
}