/*
Written by Alex Chachanashvili

$Id: ut_AStringHashMap_General.cpp 205 2008-05-29 13:58:16Z achacha $
*/
#include "pchUnitTests.hpp"
#include "AThreadPool.hpp"
#include "ASync_CriticalSection.hpp"

// Uncomment to debug to screen
//#define DEBUG_ME 1

class UTTPData : public ABase
{
public:
  UTTPData() : counter(0) {}
  ASync_CriticalSection sync;
  int counter;
};

static u4 _threadprocUnitTestThreadPool(AThread& thread)
{
  UTTPData *pData = (UTTPData *)thread.getParameter();
  thread.setRunning(true);

  {
    ALock lock(pData->sync);
    ++(pData->counter);
#ifdef DEBUG_ME
    std::cout << "[" << thread.getId() << ":" << pData->counter << "]" << std::endl;
#endif
  }

  thread.setRunning(false);
  return 0;
}

int ut_AThreadPool_General()
{
  std::cerr << "ut_AThreadPool_General" << std::endl;

  int iRet = 0x0;
  UTTPData data;
  AThreadPool pool(_threadprocUnitTestThreadPool, 10, NULL, &data);
  pool.setCreatingNewThreads(false);
  pool.setTotalThreadCreationCount(10);
  pool.start();
  pool.waitForThreadsToExit();

  ASSERT_UNIT_TEST(data.counter == 10, "AThreadPool", "0", iRet);
  return iRet;
}
