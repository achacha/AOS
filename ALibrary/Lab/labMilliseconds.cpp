#include "AString.hpp"
#include "ATime.hpp"
#include "templateAutoPtr.hpp"
#include "AThread.hpp"
#include <sys/timeb.h>

void showTime()
{
  __timeb64 tstruct;
  ::_ftime64(&tstruct);
  std::cout << tstruct.time << std::endl;
  std::cout << tstruct.millitm << std::endl;
}

int main()
{
  showTime();
  AThread::sleep(10);

  showTime();
  AThread::sleep(10);

  showTime();

  return 0;
}