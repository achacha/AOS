#include <templateASynchObject.hpp>
#include <ALock.hpp>
#include <ace/Synch.h>

#undef main
int main()
{
  ASynchObject<ACE_Thread_Mutex> synch;
  {
    ALock lock(synch);
  }
  return 0x0;
}