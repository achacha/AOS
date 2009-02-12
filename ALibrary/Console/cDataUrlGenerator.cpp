
#include "AUrl.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"
#include "AFile_Physical.hpp"

int main(int argc, char **argv)
{
  std::cout << "Data URL generator v0.1" << std::endl;
  if (argc < 3)
  {
    std::cout << "Usage: this <input picture> <output for data URL>" << std::endl;
    return -999;
  }

  try
  {
    AFilename f(ASWNL(argv[1]), false);
    if (AFileSystem::exists(f))
    {
      AFile_Physical picfile(f);
      picfile.open();

      AUrl url;
      url.useData().reserveMoreSpace(picfile.getSize() + 2);
      picfile.readUntilEOF(url.useData());
      picfile.close();

      //a_Setup the data URL
      url.setProtocol(AUrl::DATA);
      url.setIsBase64Encoded(true);
      url.useMediaType().assign(ASWNL("image/gif"));

      std::cout << url << std::endl;

      AFile_Physical dataurlfile(AFilename(ASWNL(argv[2]),false), "wb");
      dataurlfile.open();

      dataurlfile.write(ASWNL("<img src=\""));
      url.emit(dataurlfile);
      dataurlfile.writeLine(ASWNL("\"/>"));
      dataurlfile.close();
    }
    else
    {
      std::cerr << "File not found: " << f << std::endl;
      return -1;
    }
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
  }
  
  return 0;
}
