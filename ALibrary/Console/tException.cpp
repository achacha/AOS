#include "AException.hpp"

int main()
{
  try
  {
    //ATHROW(NULL, AException::OperationFailed);
    AString str("hello");
    AASSERT_EX(&str,false,ASWNL("Something bad happened"));
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
