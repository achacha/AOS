#include "ADynamicLibrary.hpp"

int main()
{
  ADynamicLibrary dll;
  if (dll.load("AOS_BaseModules"))
  {
    std::cout << "Loaded successfully." << std::endl;
  }
  else
  {
    std::cout << "NOT loaded successfully." << std::endl;
  }

  void *pProc = dll.getEntryPoint("AOS_BaseModules", "exporttest");
  
  if (pProc)
  {
    typedef int( * MYPROC)(void *);
    MYPROC myproc = (MYPROC)pProc;
    int i = myproc(NULL);
    std::cout << "i=" << i << std::endl;
  }
  else
  {
    std::cout << "Proc not found..." << std::endl;
  }

  return 0;
}
