#include <iostream>
#include <AUrl.hpp>
#include <AFile_Physical.hpp>

void writeDataUrl(AString& data)
{
  AUrl url;
  url.setProtocol("data:");
  url.useMediaType().assign("image/jpeg");
  url.setIsBase64Encoded(true);

  AFile_Physical file("WhiteHeadedMarmoset.jpg", "rb");
  file.open();
  file.readUntilEOF(url.useData());

  data = url.useData();
}

void readDataUrl(AString& data)
{
  AUrl url;
  url.parse(data);

  AFile_Physical file("WhiteHeadedMarmoset_new.jpg", "wb");
  file.open();
  file.write(url.useData());
  file.close();
}

int main()
{

//  AUrl url("http://rd.yahoo.com/M=179842.1351578.2962872.1288581/D=yahoo_top/P=xxx/S=2716149:PB/A=1070745/R=0/*http://www.hp.com");

  //AUrl url("http://me:mypass@localhost:10080/directory/filename.ext?Name0=&nAme0=value0&name1=value1&name2=value%20with%20spaces&Name3=&nAme3=1&naMe3=2&namE3=");
  //url.debugDump();
  //std::cout << url << std::endl;

  try
  {
    AString data;
    writeDataUrl(data);
    readDataUrl(data);
  }
  catch(AException& e)
  {
    std::cerr << e << std::endl;
  }
  return 0;
}

