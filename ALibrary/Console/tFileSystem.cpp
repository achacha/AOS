#include "AFilename.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"
#include "AException.hpp"
#include "AFile_Physical.hpp"
#include "AThread.hpp"
#include "ATime.hpp"
#include "AXmlElement.hpp"

void testWildcard()
{
  AFileSystem::FileInfos files;
  u4 count = AFileSystem::dir(AFilename(ASWNL("q:/ASystem/*.sbr"), false), files);
  if (count > 0)
  {
    AString str(1024, 256);
    AFileSystem::FileInfos::const_iterator cit = files.begin();
    while (cit != files.end())
    {
      cit->filename.emit(str);
      std::cout << str << std::endl;
      str.clear();
      ++cit;
    }
  }
}

void testFilesFromPath()
{
  AFileSystem::FileInfos files;
  u4 count = AFileSystem::dir(AFilename(ASWNL("d:/pmdwork/"), true), files, true, true);
  if (count > 0)
  {
    AString str(1024, 256);
    AFileSystem::FileInfos::const_iterator cit = files.begin();
    while (cit != files.end())
    {
      cit->filename.emit(str);
      if (!cit->filename.isDirectory())
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
  ATime t;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "created time =" << t << std::endl;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "last accessed=" << t << std::endl;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "modified time=" << t << std::endl;

  {
    str.clear();
    AFileSystem::getLastModifiedTime(f, t);
    t.setType(ATime::LOCAL);
    t.emitRFCtime(str);
    std::cout << "modified time=" << str << std::endl;
  }

  std::cout << "------" << std::endl;

  AThread::sleep(1000);
  ATime timeNow1;
  AFileSystem::touch(f);
  str.clear();
  timeNow1.emitRFCtime(str);
  std::cout << "timeNow1=" << str << std::endl;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "created time =" << t << std::endl;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "last accessed=" << t << std::endl;
  AFileSystem::getCreatedTime(f, t);
  std::cout << "modified time=" << t << std::endl;

  {
    str.clear();
    AFileSystem::getLastModifiedTime(f, t);
    t.setType(ATime::LOCAL);
    t.emitRFCtime(str);
    std::cout << "modified time=" << str << std::endl;
  }

  AFileSystem::remove(f);
}

void testTempFile()
{
  AFilename folder(ASWNL("./Console/samplefile.txt"), true);
  AFileSystem::generateTemporaryFilename(folder);
  std::cout << folder << std::endl;
}

void testCreateDir()
{
  AFilename path(ASWNL("Console/created1/"), false);
  AFileSystem::createDirectory(path);

  path.set(ASWNL("Console/created1/d0/d1/"), false);
  AFileSystem::createDirectory(path);

  path.set(ASWNL("Console/created1/dir0/dir1/dir2/"), false);
  AFileSystem::createDirectories(path);
}

void testCompactPath()
{
  AFilename path(ASWNL("./a/../b/../../c/"), false);   //  ../c
  path.compactPath();
  std::cout << path << std::endl;
}

void testDir()
{
  AXmlElement element("root");
  AFileSystem::FileInfos filelist;
  AFileSystem::dir(AFilename(ASWNL("c:\*.*"), false), filelist, false, true);
  for(AFileSystem::FileInfos::iterator it = filelist.begin(); it != filelist.end(); ++it)
  {
    it->emitXml(element.addElement("file"));
    std::cout << it->filename.getFilename() << ":" << it->length << ":" << it->typemask << std::endl;
  }

  ARope rope;
  element.emit(rope, 0);
  std::cout << rope << std::endl;
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
    //testTime();
    //testTempFile();
    //testCreateDir();
    //testCompactPath();
    testDir();
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