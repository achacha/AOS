#include "AFilename.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"
#include "AException.hpp"
#include "AFile_Physical.hpp"
#include "AThread.hpp"
#include "ATime.hpp"

void testWildcard()
{
  LIST_AFilename files;
  u4 count = AFileSystem::dir(AFilename(ASWNL("q:/ASystem/*.sbr"), false), files);
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
  u4 count = AFileSystem::dir(AFilename(ASWNL("d:/pmdwork/"), true), files, true, true);
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
  AFileSystem::setCWD(AFilename(ASWNL("c:\\temp\\"), true));
  AFileSystem::getCWD(f);
  std::cout << f << std::endl;
}

void testProperties()
{
  AFilename f0(ASWNL("c:\\temp\\"),true);
  if (AFileSystem::exists(f0) && AFileSystem::isA(f0, AFileSystem::Directory))
    std::cout << "c:\\temp\\ is a directory" << std::endl;
  else
    std::cout << "isA directory error" << std::endl;

  if (!AFileSystem::isA(AFilename(ASWNL("c:\\temp\\"),true), AFileSystem::File))
    std::cout << "c:\\temp\\ is not a file" << std::endl;
  else
    std::cout << "isA file error 1" << std::endl;

  if (AFileSystem::isA(AFilename(ASWNL("\\Code\\_debug\\ABase.lib"),true), AFileSystem::File))
    std::cout << "/Code/_debug/ABase.lib is a file" << std::endl;
  else
    std::cout << "isA file error 2" << std::endl;

  AFilename fTemp(ASWNL("c:\\temp\\foo1234567890.txt"), false);
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
  AFilename rel(ASWNL(".\\somefile.txt"), false);
  AFilename abs;
  AFileSystem::expand(rel, abs);
  std::cout << abs << std::endl;
}

void testLength()
{
  AFilename f(ASWNL("c:\\hiberfil.sys"), false);
  u8 l = AFileSystem::length(f);
  std::cout << l << std::endl;
}

void testTime()
{
  AString str;
  ATime timeNow0;
  AFilename f(ASWNL("testLength.txt"), false);
  AFile_Physical file(f, "w");
  file.open();
  file.write(ASWNL("Test file"));
  file.close();

  timeNow0.emitRFCtime(str);
  std::cout << "timeNow0=" << str << std::endl;
  std::cout << "created time =" << AFileSystem::createdTime(f) << std::endl;
  std::cout << "last accessed=" << AFileSystem::lastAccessedTime(f) << std::endl;
  std::cout << "modified time=" << AFileSystem::lastModifiedTime(f) << std::endl;

  {
    str.clear();
    ATime(AFileSystem::lastModifiedTime(f), ATime::LOCAL).emitRFCtime(str);
    std::cout << "modified time=" << str << std::endl;
  }

  std::cout << "------" << std::endl;

  AThread::sleep(1000);
  ATime timeNow1;
  AFileSystem::touch(f);
  str.clear();
  timeNow1.emitRFCtime(str);
  std::cout << "timeNow1=" << str << std::endl;
  std::cout << "created time =" << AFileSystem::createdTime(f) << std::endl;
  std::cout << "last accessed=" << AFileSystem::lastAccessedTime(f) << std::endl;
  std::cout << "modified time=" << AFileSystem::lastModifiedTime(f) << std::endl;

  {
    str.clear();
    ATime(AFileSystem::lastModifiedTime(f), ATime::LOCAL).emitRFCtime(str);
    std::cout << "modified time=" << str << std::endl;
  }

  AFileSystem::remove(f);
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
    //testLength();
    testTime();
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