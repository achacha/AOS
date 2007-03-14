
#include "AMutex.hpp"
#include "ALock.hpp"
#include "AFile_IOStream.hpp"

/* 
   This program is to be run from 2 (or more) separate console windows
   When one is active the other will be sitting and waiting for the mutex to free up
   Type 'exit' in one window and the next should lock the mutex
   Mutex is a Semaphore, except maxCount = 1 for mutex
*/

int main(int argc, char **argv)
{
  std::cout << "Starting program and waiting on mutex..." << std::endl;

  AMutex mutex("consoleMutex_000");
  ALock lock(mutex);
  
  std::cout << "Locking mutex..." << std::endl;

  AFile_IOStream io;
  AString input;
  while (input.compare("exit"))
  {
    std::cout << "Type 'exit' >" << std::flush;
    io.readLine(input);
  }

  std::cout << "Mutex is about to be unlocked..." << std::endl;
  return 0;
}
