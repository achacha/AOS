#include "AString.hpp"
#include "AException.hpp"

void walk_the_stack()
{
  ATHROW(AException::AssertionFailed);
}

void assert_false()
{
  AASSERT_EX(false, "Forced a false");
}

void function_zero()
{
  //walk_the_stack();
  assert_false();
}

void function_one()
{
  function_zero();
}

int main()
{
  try
  {
    function_one();
  }
  catch (AException& ex)
  {
    ARope rope;
    rope.append(ex.what());
    std::cout << rope << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown" << std::endl;
  }

  return 0;
}
