#include "pchUnitTests.hpp"
#include "AFilename.hpp"
#include "AXmlElement.hpp"

void testGeneric(int& iRet)
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
  AXmlElement root("root");
  fn.emit(root);
  strOut.clear();
  root.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("<root><AFilename type=\"0\"><drive>c</drive><path><dir>path0</dir><dir>path1</dir></path><filename>filename.extension</filename></AFilename></root>"), "AFilename XML emit", "0", iRet);

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

void testCompare(int& iRet)
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

void testValid(int& iRet)
{
  AFilename f(ASWNL("c:\\path0\\path1\\filename.ext"), false);
  ASSERT_UNIT_TEST(f.isValid(), "AFilename validity", "0", iRet);

  f.clear();
  f.useFilename().assign(")");
  ASSERT_UNIT_TEST(f.isValid(), "AFilename validity", "1", iRet);
}

int ut_AFilename_General()
{
  std::cerr << "ut_AFilename_General" << std::endl;
  int iRet = 0x0;

  testGeneric(iRet);
  testCompare(iRet);
  testValid(iRet);

  return iRet;
}
