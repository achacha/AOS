#include <templateAPathMap.hpp>
#include <AString.hpp>

#include <ace/FILE_Addr.h>
#include <ace/FILE_Io.h>
#include <ace/FILE_Connector.h>
#include <ace/FILE.h>

#undef main
int main()
{
  {
    ACE_FILE_IO file;
    ACE_FILE_Addr addrFile("D:/Output/xxx.txt");
    ACE_Addr addrSAP;
    ACE_FILE_Connector conn(file, addrFile, 0, addrSAP, 0, O_RDONLY);

    file.remove();
  }

  return 0x0;
}
