#include "ACommandLineParser.hpp"

int main(int argc, char **argv)
{
  //const int argc = 4;
  //char *argv[argc] = {
  //  "-t",
  //  "1024",
  //  "-f",
  //  "somefilename.foo"
  //};
  
  ACommandLineParser cmdParams(argc, argv);
  cmdParams.debugDump();

  return 0;
}