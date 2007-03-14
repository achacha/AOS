#include "pchABase.hpp"

#include "ABlockAllocationManager.hpp"

#include "AString.hpp"
#include "ARandomNumberGenerator.hpp"
#include "ATimer.hpp"
#include "AThread.hpp"

u4 procMy(AThread& threadThis)
{
  cerr << threadThis.getId() << endl;

  ATimer timer;
  timer.start();
  for (u4 iX = 0x0; iX < 0x2000; ++iX)
  {
//    std::string str;
//    str.reserve(ARandomNumberGenerator::intRange(16384));
    AString str(ARandomNumberGenerator::intRange(16384), 128);
  }
  timer.stop();
  cerr << "Elapsed time " << timer.getInterval() << " ms" << endl;
  
  threadThis.setFlag(AThread::Done);
  return 0x0;
}

int main()
{
  ABlockAllocationManager manager;

  const int iiCount = 12;
  AThread thread[iiCount];
  
  //aInitialize the proc for each thread
  for (int iX = 0x0; iX < iiCount; ++iX)
  {
    thread[iX].setProc(procMy);
    thread[iX].start();
  }

  int iAlive;
  do
  {
    AThread::sleep(1000);
    cerr << "!" << endl;
    
    iAlive = 0x0;
    for (int iX = 0x0; iX < iiCount; ++iX)
      if (!thread[iX].isFlag(AThread::Done))
        ++iAlive;
  }
  while(iAlive > 0x0);

/*
  u1 *pX;
  pX = manager.allocate(64);
  cout << "0x" << hex << (u4)pX << endl;
  manager.deallocate(&pX);

  pX = manager.allocate(64);
  cout << "0x" << hex << (u4)pX << endl;
  manager.deallocate(&pX);

  pX = manager.allocate(64);
  cout << "0x" << hex << (u4)pX << endl;
  manager.deallocate(&pX);

  manager.debugDump(cerr);
*/

  return 0x0;
}
