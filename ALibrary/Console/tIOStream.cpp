#include <iostream>
#include <AString.hpp>

void testIOStreamRead()
{
	char pcBuffer[100];
  std::strstream stream0;
	stream0 << "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" << std::ends;
	is.read(pcBuffer, 100);
	std::cout << "buffer = " << pcBuffer << "     read = " << is.gcount();
}

#undef main
int main()
{
  testIOStreamRead();

  return 0x0;
}