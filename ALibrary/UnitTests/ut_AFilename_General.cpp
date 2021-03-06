/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include "AFilename.hpp"
#include "AXmlElement.hpp"

void testAFilenameGeneric(int& iRet)
{
  AString strOut(1024, 256);
  AFilename fn(ASWNL("c:\\path0\\path1//filename.extension"), false);

  //a_Emit based on format
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:/path0/path1/filename.extension"), "AFilename emit", "0", iRet);
  
  fn.setType(AFilename::FTYPE_MSDOS);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:\\path0\\path1\\filename.extension"), "AFilename emit", "1", iRet);

  fn.setType(AFilename::FTYPE_UNIX);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("/path0/path1/filename.extension"), "AFilename emit", "2", iRet);

  fn.setType(AFilename::FTYPE_CYGWIN);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("/cygdrive/c/path0/path1/filename.extension"), "AFilename emit", "3", iRet);
  fn.setType(AFilename::FTYPE_DEFAULT);

  //a_Xml emit
  AXmlElement root("filename");
  fn.emitXml(root);
  strOut.clear();
  root.emit(strOut);
  //std::cout << strOut << std::endl;
  ASSERT_UNIT_TEST(strOut.equals("<filename os=\"0\"><drive>c</drive><path><dir>path0</dir><dir>path1</dir></path><name>filename.extension</name><full>c:/path0/path1/filename.extension</full></filename>"), "AFilename XML emit", "0", iRet);

  //a_Copy
  fn.set(ASWNL("c:/some/path/here/file.ext"), false);
  AFilename fn1(fn);
  AString str1;
  strOut.clear();
  fn.emit(strOut);
  fn.emit(str1);
  ASSERT_UNIT_TEST(strOut.equals(str1), "AFilename copy", "0", iRet);

  //a_Extension
  fn.set(ASWNL("c:/some/path/here/file.ext"), false);
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "0", iRet);
  ASSERT_UNIT_TEST(strOut.equals("ext"), "AFilename get extension", "1", iRet);

  fn.setExtension(ASW("xml",3));
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "2", iRet);
  ASSERT_UNIT_TEST(strOut.equals("xml"), "AFilename get extension", "3", iRet);

  fn.setExtension(ASW("ext",3));
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "4", iRet);
  ASSERT_UNIT_TEST(strOut.equals("ext"), "AFilename get extension", "5", iRet);

  //a_Join
  fn.set(ASWNL("c:/some/path/here/file.ext"), false);
  fn1.set(ASWNL("d:my/path/new.new"), false);
  strOut.clear();
  fn.join(fn1);
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("d:/some/path/here/my/path/new.new"), "AFilename join", "0", iRet);

  fn.set(ASWNL("c:/some/path/here/"), false);
  fn1.set(ASWNL("new.new"), false);
  strOut.clear();
  fn.join(fn1);
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:/some/path/here/new.new"), "AFilename join", "0", iRet);

  //a_Set
  fn.set(ASWNL("c:\\path0\\path1//filename.extension"), false);
  ASSERT_UNIT_TEST(!fn.isRelativePath(), "AFilename set", "0", iRet);
  ASSERT_UNIT_TEST(!fn.isDirectory(), "AFilename set", "1", iRet);

  fn.set(ASWNL("/some/path"), true);
  ASSERT_UNIT_TEST(!fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(fn.isDirectory(), "AFilename set", "3", iRet);

  fn.set(ASWNL("some/path.foo"), true);
  ASSERT_UNIT_TEST(fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(fn.isDirectory(), "AFilename set", "3", iRet);

  fn.set(ASWNL("some/path.foo"), false);
  ASSERT_UNIT_TEST(fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(!fn.isDirectory(), "AFilename set", "3", iRet);
}

void testAFilenameCompare(int& iRet)
{
  AFilename fOne(ASWNL("c:\\path0\\path1\\filename.ext"), false);
  AFilename fTwo(ASWNL("c:\\path0\\path1\\filename.ext"), false);
  ASSERT_UNIT_TEST(!(fOne < fTwo), "AFilename operator <", "0", iRet);
  ASSERT_UNIT_TEST(fOne == fTwo, "AFilename operator ==", "0", iRet);

  fTwo.set(ASWNL("c:/path0/path1/filename.ext"), false);
  ASSERT_UNIT_TEST(!(fOne < fTwo), "AFilename operator <", "1", iRet);
  ASSERT_UNIT_TEST(fOne == fTwo, "AFilename operator ==", "1", iRet);

  fTwo.set(ASWNL("/relative"), false);
  ASSERT_UNIT_TEST(fTwo < fOne, "AFilename operator <", "2", iRet);

  fOne.set(ASWNL("/relative"), false);
  ASSERT_UNIT_TEST(fOne == fTwo, "AFilename operator ==", "2", iRet);

  fOne.set(ASWNL("filename.ext0"), false);
  fTwo.set(ASWNL("filename.ext1"), false);
  ASSERT_UNIT_TEST(fOne < fTwo, "AFilename operator <", "3", iRet);
}

void testAFilenameValid(int& iRet)
{
  AFilename f(ASWNL("c:\\path0\\path1\\filename.ext"), false);
  ASSERT_UNIT_TEST(f.isValid(), "AFilename validity", "0", iRet);

  f.clear();
  f.useFilename().assign(")");
  ASSERT_UNIT_TEST(f.isValid(), "AFilename validity", "1", iRet);
}

void testAFilenameManipulate(int& iRet)
{
  AFilename f(ASWNL("/foo/bar/baz/filename.ext0.ext1.ext2.ext3"), false);
  f.removeExtension(4);
  ASSERT_UNIT_TEST(f.useFilename().equals(ASWNL("filename")), "AFilename::removeExtension", "0", iRet);

  AString str;
  AFilename fBase(ASWNL("/foo/bar"), true);
  f.removeBasePath(fBase);
  f.emit(str);
  ASSERT_UNIT_TEST(str.equals(ASWNL("baz/filename")), "AFilename::removeBasePath", "0", iRet);
  ASSERT_UNIT_TEST(f.isRelativePath(), "AFilename::removeBasePath", "1", iRet);
}

void testAFilenameConversion(int& iRet)
{
  {
    AFilename f(ASWNL("c:\\dir0\\dir1\\myfile"), false);
    AString str;
    f.emit(str);
    ASSERT_UNIT_TEST(str.equals(ASWNL("c:/dir0/dir1/myfile")), "AFilename::conversion", "0", iRet);

    f.makeRelative();
    str.clear();
    f.emit(str);
    ASSERT_UNIT_TEST(str.equals(ASWNL("./dir0/dir1/myfile")), "AFilename::conversion", "1", iRet);

    f.makeAbsolute();
    str.clear();
    f.emit(str);
    ASSERT_UNIT_TEST(str.equals(ASWNL("c:/dir0/dir1/myfile")), "AFilename::conversion", "2", iRet);

    f.setType(AFilename::FTYPE_MSDOS);
    str.clear();
    f.emit(str);
    ASSERT_UNIT_TEST(str.equals(ASWNL("c:\\dir0\\dir1\\myfile")), "AFilename::conversion", "3", iRet);
  }
}

void testAFilenameCompact(int& iRet)
{
  AString str("./../foo/../bar/../baz/");
  AFilename f(str, true);
  ASSERT_UNIT_TEST(false == f.compactPath(), "AFilename::compactPath", "0", iRet);
  str.clear();
  f.emit(str);
  ASSERT_UNIT_TEST(str.equals(ASWNL("../baz/")), "AFilename::compactPath", "1", iRet);

  str.assign("../../foo/bar/baz/.././gaz/");
  f.clear();
  f.set(str, false);
  ASSERT_UNIT_TEST(false == f.compactPath(), "AFilename::compactPath", "2", iRet);
  str.clear();
  f.emit(str);
  ASSERT_UNIT_TEST(str.equals(ASWNL("../../foo/bar/gaz/")), "AFilename::compactPath", "3", iRet);

  str.assign("foo/bar/../../../baz/");
  f.clear();
  f.set(str, false);
  ASSERT_UNIT_TEST(false == f.compactPath(), "AFilename::compactPath", "2", iRet);
  str.clear();
  f.emit(str);
  ASSERT_UNIT_TEST(str.equals(ASWNL("../baz/")), "AFilename::compactPath", "3", iRet);

  str.assign("foo/bar/.././baz/../gaz/eee/");
  f.clear();
  f.set(str, false);
  ASSERT_UNIT_TEST(true == f.compactPath(), "AFilename::compactPath", "2", iRet);
  str.clear();
  f.emit(str);
  ASSERT_UNIT_TEST(str.equals(ASWNL("foo/gaz/eee/")), "AFilename::compactPath", "3", iRet);
}

int ut_AFilename_General()
{
  std::cerr << "ut_AFilename_General" << std::endl;
  int iRet = 0x0;

  testAFilenameGeneric(iRet);
  NEWLINE_UNIT_TEST();
  testAFilenameCompare(iRet);
  NEWLINE_UNIT_TEST();
  testAFilenameValid(iRet);
  NEWLINE_UNIT_TEST();
  testAFilenameManipulate(iRet);
  NEWLINE_UNIT_TEST();
  testAFilenameCompact(iRet);
  NEWLINE_UNIT_TEST();
  testAFilenameConversion(iRet);

  return iRet;
}
