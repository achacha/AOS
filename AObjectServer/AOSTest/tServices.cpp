#include "AFilename.hpp"
#include "AOSServices.hpp"

int main()
{
  AFilename f(ASWNL("../aos_root/"), true);
  AAutoPtr<AOSServices> pservices(new AOSServices(f), true);
  
  return 0;
}