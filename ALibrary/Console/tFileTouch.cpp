#include <templateAPathMap.hpp>
#include <AString.hpp>
#include <AFileSystemACEWrappers.hpp>

#undef main
int main()
{
  AString strF("D:/Output/foo.txt");
  if (AFileSystemACEWrappers::exists(strF))
  {
    cout << "File 'foo.txt' exists, removing!" << endl;
    AFileSystemACEWrappers::remove(strF);
  }
  else
  {
    cout << "File 'foo.txt' does NOT exist, touching!" << endl;
    AFileSystemACEWrappers::touch(strF);
  }

  return 0x0;
}
