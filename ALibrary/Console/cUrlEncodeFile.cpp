#include "AFile_Physical.hpp"
#include "ATextConverter.hpp"

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: this <input file> <output file>" << std::endl;
    return -1;
  }

  const int iiBufferSize = 128;
  AString strInFile(argv[1]), strOutFile(argv[2]);
  AFile_Physical fileIn, fileOut;
  AString str(iiBufferSize, 1024);

  if (fileIn.open(strInFile, "rb"))
  {
    if (fileOut.open(strOutFile, "wb"))
    {
      while(fileIn.read(str, iiBufferSize))
      {
        fileOut.write(ATextConverter::encodeURL(str));
      }
      fileOut.close();
    }
    else
      std::cerr << "Unable to open for write: " << strOutFile << std::endl;

    fileIn.close();
  }
  else
    std::cerr << "Unable to open for read: " << strOutFile << std::endl;
}

