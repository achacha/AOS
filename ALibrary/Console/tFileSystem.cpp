#include "AFilename.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"
#include "AException.hpp"

void testWildcard()
{
  LIST_AFilename files;
  u4 count = AFileSystem::dir(AFilename("q:/ASystem/*.sbr"), files);
  if (count > 0)
  {
    AString str(1024, 256);
    LIST_AFilename::const_iterator cit = files.begin();
    while (cit != files.end())
    {
      (*cit).emit(str);
      std::cout << str << std::endl;
      str.clear();
      ++cit;
    }
  }
}

void testFilesFromPath()
{
  LIST_AFilename files;
  u4 count = AFileSystem::dir(AFilename("d:/pmdwork/"), files, true, true);
  if (count > 0)
  {
    AString str(1024, 256);
    LIST_AFilename::const_iterator cit = files.begin();
    while (cit != files.end())
    {
      (*cit).emit(str);
      if (!(*cit).isDirectory())
        std::cout << "F: " << str << std::endl;
      else
        std::cout << "DIR:  " << str << std::endl;
      str.clear();
      ++cit;
    }
  }
}

void testCWD()
{
  AFilename f;
  AFileSystem::getCWD(f);
  std::cout << f << std::endl;
  AFileSystem::setCWD(AFilename("c:\\temp\\", true));
  AFileSystem::getCWD(f);
  std::cout << f << std::endl;
}

void testProperties()
{
  AFilename f0("c:\\temp\\",true);
  if (AFileSystem::exists(f0) && AFileSystem::isA(f0, AFileSystem::Directory))
    std::cout << "c:\\temp\\ is a directory" << std::endl;
  else
    std::cout << "isA directory error" << std::endl;

  if (!AFileSystem::isA(AFilename("c:\\temp\\",true), AFileSystem::File))
    std::cout << "c:\\temp\\ is not a file" << std::endl;
  else
    std::cout << "isA file error 1" << std::endl;

  if (AFileSystem::isA(AFilename("\\Code\\_debug\\ABase.lib",true), AFileSystem::File))
    std::cout << "/Code/_debug/ABase.lib is a file" << std::endl;
  else
    std::cout << "isA file error 2" << std::endl;

  AFilename fTemp("c:\\temp\\foo1234567890.txt");
  if (AFileSystem::exists(fTemp))
  {
    AFileSystem::remove(fTemp);
    if (AFileSystem::exists(fTemp))
      std::cout << "File remove failed." << std::endl;
    else
      std::cout << "File removed." << std::endl;
  }
  AFileSystem::touch(fTemp);
  if (AFileSystem::exists(fTemp))
    std::cout << "File touched." << std::endl;
  else
    std::cout << "Unable to touch" << std::endl;
}

void testRelative()
{
  AFilename rel(".\\somefile.txt");
  AFilename abs;
  AFileSystem::expand(rel, abs);
  std::cout << abs << std::endl;
}

void testLength()
{
  AFilename f("c:\\hiberfil.sys");
  u8 l = AFileSystem::length(f);
  std::cout << l << std::endl;
}

int main()
{
  try
  {
    //testWildcard();
    //testFilesFromPath();
    //testCWD();
    //testProperties();
    //testRelative();
    testLength();
  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

  return 0;
}