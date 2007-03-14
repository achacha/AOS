#include "ALog_AFile.hpp"
#include "ASynch_NOP.hpp"
#include "AFile_AString.hpp"
#include "AThread.hpp"

void testScope(ALog& alog)
{
  ALog::ScopeEvents& se = alog.addScopeEvents("testScope");
  se.add(ASW("event 0",7), ALog::WARNING);
  se.add(ASW("error 0",7), ALog::CRITICAL);
  AThread::sleep(100);
  se.add(ASW("time 100",8), ALog::TIMING);
  AThread::sleep(200);
  se.add(ASW("time 200",8), ALog::TIMING);
  AThread::sleep(300);
  se.add(ASW("time 300",8), ALog::TIMING);
}

int main()
{
  AFile_AString *pStrFile = new AFile_AString();
  ALog_AFile alog(new ASynch_NOP(), pStrFile);

  alog.add(AString("Warning at start"), ALog::WARNING);
  alog.add(AString("Starting logging"), ALog::STATISTIC);
  testScope(alog);
  alog.add(AString("Finished logging"), ALog::STATISTIC);

  std::cout << pStrFile->useAString() << std::endl;

  return 0;
}