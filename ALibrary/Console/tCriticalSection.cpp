
#include <ALock.hpp>
#include <ACriticalSection.hpp>

int main()
{
  ACriticalSection cs;
  ALock lock(cs);

  return 0x0;
}
