#include <iostream>
#include <AForm.hpp>

int main()
{
/*
  AForm form;
  
  form.parse("a=1&a=2&a=3&b=4");
  VECTOR_AString sv;
  form.find("a", sv);
*/
  int N = 4, p =1,  k =0;
  while (k < N)
  {
    p = 2 * p;
    k = k+ 1;
  }
    
  std::cout << "size=" << p << std::endl;
  
  return 0;
}