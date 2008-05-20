
#include "ABase.hpp"
#include "templateVectorOfPtrs.hpp"
#include "ABasePtrQueue.hpp"
#include "AString.hpp"

class Echo : public ABase
{
public:
  Echo() { std::cout << "ctor" << std::endl; }
  ~Echo() { std::cout << "dtor" << std::endl; }
};

void testEcho()
{
  AVectorOfPtrs<ABase> v;
  v.push_back(new Echo());
}

void testQueue()
{
  ABasePtrQueue q;
  q.push(new Echo());
  q.clear();
}

int main()
{
  //testEcho();
  testQueue();
  return 0x0;
}