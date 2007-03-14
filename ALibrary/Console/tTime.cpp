#include "ATime.hpp"
#include <stdio.h>
#include "AException.hpp"

void testStrftime()
{
  ATime t0, t1(ATime::LOCAL);
  printf("%s", t0.strftime(AString("GMT0 is %A, day %d of %B in the year %Y.\n")).c_str());
  printf("%s", t0.strftime(AString("GMT0 is %A, day %d of %B in the year %Y.\n")).c_str());
  printf("%s", t1.strftime(AString("LOC0 is %A, day %d of %B in the year %Y.\n")).c_str());
  printf("%s", t1.strftime(AString("LOC0 is %A, day %d of %B in the year %Y.\n")).c_str());

}

void testGmtime()
{
  time_t tt;
  ::time(&tt);
  tm* ptmX;
  int iSize = 1024;
  char *p;
  
  ptmX = ::gmtime(&tt);
  p = new char[iSize];
  ::strftime(p, iSize - 0x1, "GMT0 is %A, day %d of %B in the year %Y.\n", ptmX);
  printf("%s", p);
  delete[] p;
  delete ptmX;

  ptmX = ::gmtime(&tt);
  p = new char[iSize];
  ::strftime(p, iSize - 0x1, "GMT0 is %A, day %d of %B in the year %Y.\n", ptmX);
  printf("%s", p);
  delete[] p;
  delete ptmX;
}

void testTimeMath()
{
  time_t tt;
  tt = 1;    //a_Milliseconds
  ATime t0, t1(tt);

  printf("%s", t0.strftime(AString("%H:%M:%S, GMT0 is %A, day %d of %B in the year %Y.\n")).c_str());
  t0 += t1;
  printf("%s", t0.strftime(AString("%H:%M:%S, GMT0 is %A, day %d of %B in the year %Y.\n")).c_str());
  t0 -= t1;
  printf("%s", t0.strftime(AString("%H:%M:%S, GMT0 is %A, day %d of %B in the year %Y.\n")).c_str());

}

void test_toRFCtime()
{
  ATime t;
  printf("now =%s\n", t.toRFCtime().c_str());

  t.set((time_t)0x0);
  printf("then=%s\n", t.toRFCtime().c_str());
}

void testTimeNow()
{
  ATime timeNow1;
  cout << "GMT=" << timeNow1.toRFCtime().c_str() << endl;

  ATime timeNow2(ATime::LOCAL);
  cout << "LOCAL=" << timeNow2.toRFCtime().c_str() << endl;
}

void test_fromRFCtime_parse()
{
  AString strT("Thu, 28-Jan-2000 18:00 UT-5");
  ATime time;
  time.fromRFCtime(strT);
  cout << strT.c_str() << endl;
  cout << time.toRFCtime().c_str() << endl;
}

void test_fromRFCtime_convert()
{
  AString strT("Thu, 28-Jan-2000 18:00 UT-5");
  ATime t(ATime::LOCAL);
  t.fromRFCtime(strT);
  cout << "before parse=" << strT.c_str() << endl;
  cout << "original LOCAL=" << t.toRFCtime() << endl;
  t.setType(ATime::UT);
  cout << "converted UT=" << t.toRFCtime() << endl;
}

void testTimezone()
{
  cout << "UT-9  = " << ATime::getTimezoneFromString("UT-9") << endl;
  cout << "UT    = " << ATime::getTimezoneFromString("UT") << endl;
  cout << "UT+7  = " << ATime::getTimezoneFromString("UT+7") << endl;
  cout << "GMT-5 = " << ATime::getTimezoneFromString("GMT-5") << endl;
  cout << "+1300 = " << ATime::getTimezoneFromString("+1300") << endl;
  cout << "-0630 = " << ATime::getTimezoneFromString("-0630") << endl;
  cout << "W     = " << ATime::getTimezoneFromString("W") << endl;
  cout << "e     = " << ATime::getTimezoneFromString("e") << endl;
  cout << "EST   = " << ATime::getTimezoneFromString("EST") << endl;
  cout << "PDT   = " << ATime::getTimezoneFromString("PDT") << endl;
  cout << "MST   = " << ATime::getTimezoneFromString("MST") << endl;
  cout << "CDT   = " << ATime::getTimezoneFromString("CDT") << endl;
}

void testLocalTime()
{
  ATime t(ATime::UT);
  ATime t1(ATime::LOCAL);

  cout << "original UT=" << t.toRFCtime() << endl;
  t.setType(ATime::LOCAL);
  cout << "converted LOCAL=" << t.toRFCtime() << endl;

  cout << "original LOCAL=" << t1.toRFCtime() << endl;
  t1.setType(ATime::UT);
  cout << "converted UT=" << t1.toRFCtime() << endl;
}


int main()
{
  try
  {
  //  testStrftime();
  //  testTimeMath();
  //  testTimeNow();
  //  test_toRFCtime();
  //  test_fromRFCtime_parse();
    test_fromRFCtime_convert();
  //  testTimezone();
  //  testLocalTime();
  }
  catch(AException& e)
  {
    cerr << e.what() << endl;
  }

  return 0x0;
}
