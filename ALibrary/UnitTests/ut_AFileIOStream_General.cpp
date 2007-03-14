#include "pchUnitTests.hpp"

#include <AFile_IOStream.hpp>
#include <AException.hpp>
#include <sstream>

int ut_AFileIOStream_General()
{
  std::cerr << "ut_AFileIOStream_General" << std::endl;

  int iRet = 0x0;

  std::stringstream stream0;
	stream0 << "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" << std::ends;
  AFile_IOStream fileStream(stream0, stream0);
  
  int iRead = 0x0;
  char pcBuffer[128];
  
  //a_First try peek
  iRead = fileStream.peek(pcBuffer, 10);
  pcBuffer[iRead] = '\x0';
  if (iRead != 10)
  {
    std::cerr << "Invalid 10 byte peek count" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;
  if (strcmp(pcBuffer, "0123456789"))
  {
    std::cerr << "Invalid 10 byte peek" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Read data entirely in lookahead
  iRead = fileStream.read(pcBuffer, 3);
  pcBuffer[iRead] = '\x0';
  if (iRead != 3)
  {
    std::cerr << "Invalid 3 byte read count" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;
  if (strcmp(pcBuffer, "012"))
  {
    std::cerr << "Invalid 3 byte read" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Now read data that uses both lookahead and input stream
  iRead = fileStream.read(pcBuffer, 8);
  pcBuffer[iRead] = '\x0';
  if (iRead != 8)
  {
    std::cerr << "Invalid 8 byte read count" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;
  if (strcmp(pcBuffer, "3456789A"))
  {
    std::cerr << "Invalid 8 byte read" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Peek more
  iRead = fileStream.peek(pcBuffer, 10);
  pcBuffer[iRead] = '\x0';
  if (iRead != 10)
  {
    std::cerr << "Invalid 10 byte peek count" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;
  if (strcmp(pcBuffer, "BCDEFGHIJK"))
  {
    std::cerr << "Invalid 8 byte peek" << std::endl;
  } else std::cerr << "." << std::flush;
  
  //a_Read more
  iRead = fileStream.read(pcBuffer, 18);
  pcBuffer[iRead] = '\x0';
  if (iRead != 18)
  {
    std::cerr << "Invalid 18 byte read count" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;
  if (strcmp(pcBuffer, "BCDEFGHIJKLMNOPQRS"))
  {
    std::cerr << "Invalid 18 byte read" << std::endl;
  } else std::cerr << "." << std::flush;
  
  //a_Check for exception overread (should not occur)
  try
  {
    iRead = fileStream.read(pcBuffer, 100);
    pcBuffer[iRead] = '\x0';
    if (iRead != 34)
    {
      std::cerr << "Invalid read to EOF" << std::endl;
      iRet++;
    } else std::cerr << "." << std::flush;
    if (strcmp(pcBuffer, "TUVWXYZabcdefghijklmnopqrstuvwxyz"))
    {
      std::cerr << "Invalid content in EOF read" << std::endl;
      iRet++;
    } else std::cerr << "." << std::flush;
  }
  catch(...)
  {
    std::cerr << "Unhandled exception." << std::endl;
    iRet++;
  }

  //a_Read of empty
  if (iRead = fileStream.read(pcBuffer, 1))
  {
    iRet++;
    std::cerr << "Data read after stream is empty" << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}
