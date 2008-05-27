/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include <ATimer.hpp>

#if defined(__WIN32__)
#include <windows.h>
#else
#endif

int ut_ATimer_General()
{
  std::cerr << "ut_ATimer_General" << std::endl;

  int iRet = 0x0;

  //a_Creation
  ATimer timer;
  if (timer.getInterval() != 0)
  {
    iRet++;
    std::cerr << "Construction invalid: " << timer.getInterval() << "!=0" << std::endl;
  } else std::cerr << "." << std::flush;


#if defined(__WINDOWS__)
  //a_Delay validation (rough check of functionality)
  u4 uStart = ::GetTickCount();
  timer.start();
  while (::GetTickCount() - uStart < 500)
  {
    ::Sleep(50);
  }
  timer.stop();
  if (timer.getInterval() < 400 || timer.getInterval() > 600)
  {
    iRet++;
    std::cerr << "Interval duraction invalid: " << timer.getInterval() << " should be 1000+-100" << std::endl;
  } else std::cerr << "." << std::flush;
#else
#endif

  //a_Check reset
  timer.clear();
  if (timer.getInterval() != 0)
  {
    iRet++;
    std::cerr << "Reset invalid: " << timer.getInterval() << "!=0" << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}