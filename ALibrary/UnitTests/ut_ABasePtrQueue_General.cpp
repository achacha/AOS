/*
Written by Alex Chachanashvili

$Id: ut_ABitArray_General.cpp 205 2008-05-29 13:58:16Z achacha $
*/
#include "pchUnitTests.hpp"
#include "ABasePtrQueue.hpp"

void testSimplePushBackPopFront(int& iRet)
{
  ABasePtrQueue queue;

  AString *pStr000 = new AString("test_000");
  queue.pushBack(pStr000);
  ASSERT_UNIT_TEST(1 == queue.size(), "push back, size", "0", iRet);
  ASSERT_UNIT_TEST(pStr000 == queue.useHead(), "push back, head", "0", iRet);
  ASSERT_UNIT_TEST(pStr000 == queue.useTail(), "push back, tail", "0", iRet);

  ABase *pBase000 = queue.popFront();
  AString *pStrPop000 = dynamic_cast<AString *>(pBase000);
  ASSERT_UNIT_TEST(pStr000 == pStrPop000, "push back, pop test", "0", iRet);
  ASSERT_UNIT_TEST(0 == queue.size(), "push back, pop test, size", "0", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useHead(), "push back, pop test, queue head", "0", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useTail(), "push back, pop test, queue tail", "0", iRet);
  ASSERT_UNIT_TEST(NULL == pStrPop000->getPrev(), "push back, pop test, obj prev", "0", iRet);
  ASSERT_UNIT_TEST(NULL == pStrPop000->getNext(), "push back, pop test, obj next", "0", iRet);

  delete pStrPop000;
}

void testSimplePushFrontPopBack(int& iRet)
{
  ABasePtrQueue queue;

  AString *pStr000 = new AString("test_000");
  queue.pushFront(pStr000);
  ASSERT_UNIT_TEST(1 == queue.size(), "push front, size", "0", iRet);
  ASSERT_UNIT_TEST(pStr000 == queue.useHead(), "push front, head", "0", iRet);
  ASSERT_UNIT_TEST(pStr000 == queue.useTail(), "push front, tail", "0", iRet);

  AString *pStrPop000 = dynamic_cast<AString *>(queue.popBack());
  ASSERT_UNIT_TEST(pStr000 == pStrPop000, "push front, pop test", "0", iRet);
  ASSERT_UNIT_TEST(0 == queue.size(), "push front, pop test, size", "0", iRet);
  ASSERT_UNIT_TEST(queue.isEmpty(), "push front, pop test, size", "1", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useHead(), "push front, pop test, queue head", "0", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useTail(), "push front, pop test, queue tail", "0", iRet);
  ASSERT_UNIT_TEST(NULL == pStrPop000->getPrev(), "push front, pop test, obj prev", "0", iRet);
  ASSERT_UNIT_TEST(NULL == pStrPop000->getNext(), "push front, pop test, obj next", "0", iRet);

  delete pStrPop000;
}

void testClearAndRelease(int& iRet)
{
  ABasePtrQueue queue;
  queue.pushFront(new AString("3"));
  queue.pushFront(new AString("2"));
  queue.pushFront(new AString("1"));
  queue.pushBack(new AString("4"));
  queue.pushBack(new AString("5"));
  queue.pushBack(new AString("6"));

  AString str;
  ABase *p = queue.useHead();
  while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
  ASSERT_UNIT_TEST(str.equals("123456"), "clear and release, content", "0", iRet);

  queue.clear(true);
  ASSERT_UNIT_TEST(0 == queue.size(), "clear and release, size", "0", iRet);
  ASSERT_UNIT_TEST(queue.isEmpty(), "clear and release, size", "1", iRet);
}

void testFind(int& iRet)
{
  ABasePtrQueue queue;
  ABase *f2 = queue.pushFront(new AString("3"));
  ABase *f1 = queue.pushFront(new AString("2"));
  ABase *f0 = queue.pushFront(new AString("1"));
  ABase *f3 = queue.pushBack(new AString("4"));
  ABase *f4 = queue.pushBack(new AString("5"));
  ABase *f5 = queue.pushBack(new AString("6"));
  AString *p = new AString("not in queue");
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(p), "find", "-1", iRet);
  ASSERT_UNIT_TEST(0 == queue.findFromFront(f0), "find", "0", iRet);
  ASSERT_UNIT_TEST(1 == queue.findFromFront(f1), "find", "1", iRet);
  ASSERT_UNIT_TEST(2 == queue.findFromFront(f2), "find", "2", iRet);
  ASSERT_UNIT_TEST(3 == queue.findFromFront(f3), "find", "3", iRet);
  ASSERT_UNIT_TEST(4 == queue.findFromFront(f4), "find", "4", iRet);
  ASSERT_UNIT_TEST(5 == queue.findFromFront(f5), "find", "5", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("123456"), "find content", "5a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("654321"), "find content", "5b", iRet);
  }

  delete queue.remove(f2);
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f2), "find", "6", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("12456"), "find content", "6a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("65421"), "find content", "6b", iRet);
  }

  delete queue.remove(f5);
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f5), "find", "7", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("1245"), "find content", "7a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("5421"), "find content", "7b", iRet);
  }

  delete queue.remove(f0);
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f0), "find", "8", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("245"), "find content", "8a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("542"), "find content", "8b", iRet);
  }

  delete queue.popFront();
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f0), "find", "9", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("45"), "find content", "9a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("54"), "find content", "9b", iRet);
  }

  delete queue.popBack(1);
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f0), "find", "10", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("5"), "find content", "10a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("5"), "find content", "10b", iRet);
  }

  delete queue.popBack();
  ASSERT_UNIT_TEST(AConstant::npos == queue.findFromFront(f0), "find", "11", iRet);
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.isEmpty(), "find content", "11a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.isEmpty(), "find content", "11b", iRet);
  }

  ASSERT_UNIT_TEST(0 == queue.size(), "find final size", "0", iRet);
  ASSERT_UNIT_TEST(queue.isEmpty(), "find final size", "1", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useHead(), "find final queue head", "0", iRet);
  ASSERT_UNIT_TEST(NULL == queue.useTail(), "find final queue tail", "0", iRet);
}

void testRandomAccess(int& iRet)
{
  ABasePtrQueue queue;
  queue.pushFront(new AString("2"));
  queue.pushBack(new AString("3"));
  queue.pushFront(new AString("1"));
  queue.pushBack(new AString("4"));
  queue.pushFront(new AString("0"));
  queue.pushBack(new AString("5"));
  {
    AString str;
    ABase *p = queue.useHead();
    while (p) { ((AString *)p)->emit(str); p = p->useNext(); }
    ASSERT_UNIT_TEST(str.equals("012345"), "random access", "0a", iRet);
  }
  {
    AString str;
    ABase *p = queue.useTail();
    while (p) { ((AString *)p)->emit(str); p = p->usePrev(); }
    ASSERT_UNIT_TEST(str.equals("543210"), "random access", "0b", iRet);
  }

  AString *pstr = dynamic_cast<AString *>(queue.useAt(5));
  ASSERT_UNIT_TEST(pstr->equals("5"), "useAt", "0", iRet);
  pstr = dynamic_cast<AString *>(queue.useAt(0));
  ASSERT_UNIT_TEST(pstr->equals("0"), "useAt", "1", iRet);
  pstr = dynamic_cast<AString *>(queue.useAt(3));
  ASSERT_UNIT_TEST(pstr->equals("3"), "useAt", "2", iRet);

  const AString *pcstr = dynamic_cast<const AString *>(queue.getAt(2));
  ASSERT_UNIT_TEST(pcstr->equals("2"), "getAt", "0", iRet);
  pcstr = dynamic_cast<const AString *>(queue.getAt(1));
  ASSERT_UNIT_TEST(pcstr->equals("1"), "getAt", "1", iRet);
  pcstr = dynamic_cast<const AString *>(queue.getAt(4));
  ASSERT_UNIT_TEST(pcstr->equals("4"), "getAt", "2", iRet);
}

int ut_ABasePtrQueue_General(void)
{
  int iRet = 0x0;

  std::cerr << "ut_ABasePtrQueue_General" << std::endl;

  testSimplePushBackPopFront(iRet);
  NEWLINE_UNIT_TEST();
  testSimplePushFrontPopBack(iRet);
  NEWLINE_UNIT_TEST();
  testClearAndRelease(iRet);
  NEWLINE_UNIT_TEST();
  testFind(iRet);
  NEWLINE_UNIT_TEST();
  testRandomAccess(iRet);

  return iRet;
}
