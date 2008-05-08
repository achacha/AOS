#include "templateVectorOfPtrs.hpp"
#include "ABase.hpp"

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

int main()
{
  testEcho();
  return 0x0;
}