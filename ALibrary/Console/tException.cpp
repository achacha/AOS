#include "AException.hpp"

int main()
{
  try
  {
    ATHROW(NULL, AException::OperationFailed);
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown?" << std::endl;
  }

  return 0;
}
