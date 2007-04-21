#include "AFilename.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"

void testParse()
{
  AFilename fn("c:\\path0\\path1//filename.extension");
  AString strOut(1024, 256);

  fn.emit(strOut);
  std::cout << strOut << std::endl;

  AXmlElement root("root");
  fn.emit(root);
  strOut.clear();
  root.emit(strOut);
  std::cout << strOut << std::endl;
}

void testFormatting()
{
  AString strOut(1024, 256);
  AFilename fn("c:\\path0\\path1//filename.extension");

  fn.setType(AFilename::FTYPE_MSDOS);
  fn.emit(strOut);
  std::cout << strOut << std::endl;

  strOut.clear();
  fn.setType(AFilename::FTYPE_UNIX);
  fn.emit(strOut);
  std::cout << strOut << std::endl;

  strOut.clear();
  fn.setType(AFilename::FTYPE_CYGWIN);
  fn.emit(strOut);
  std::cout << strOut << std::endl;
}

void testCopy()
{
  AString strOut(1024, 256);
  AFilename fn0("c:/some/path/here/file.ext");
  AFilename fn1(fn0);

  fn0.emit(strOut);
  std::cout << strOut << std::endl;

  strOut.clear();
  fn1.emit(strOut);
  std::cout << strOut << std::endl;
}

void testJoin()
{
  AString strOut(1024, 256);
  AFilename fn0("c:/some/path/here/file.ext");
  AFilename fn1("d:my/path/new.new");

  fn0.join(fn1);

  fn0.emit(strOut);
  std::cout << strOut << std::endl;
}

void testExt()
{
  //AFilename fn("/path/file.ext");
  AString str(16,16);

  //if (fn.getExtension(str))
  //{
  //  std::cout << "Extension: " << str << std::endl;
  //}
  //else
  //{
  //  std::cout << "Extension not found." << std::endl;
  //}

  //fn.set("/path2/path1/foo");
  //str.clear();
  //if (fn.getExtension(str))
  //{
  //  std::cout << "Extension: " << str << std::endl;
  //}
  //else
  //{
  //  std::cout << "Extension not found." << std::endl;
  //}


  AFilename fn("/path2/path1/foo.");
  str.clear();
  if (fn.hasExtension())
  {
    fn.emitExtension(str);
    std::cout << "Extension: " << str << std::endl;
  }
  else
  {
    std::cout << "Extension not found." << std::endl;
  }
}

void testRelativePath()
{
  AFilename fn("/absolute/path", true);
  if (fn.isRelativePath())
  {
    std::cout << "Relative" << std::endl;
  }
  else
  {
    std::cout << "Absolute" << std::endl;
  }
  if (fn.isDirectory())
  {
    std::cout << "Dir" << std::endl;
  }
  else
  {
    std::cout << "Not Dir" << std::endl;
  }

  fn.set("relative/file.txt");
  if (fn.isRelativePath())
  {
    std::cout << "Relative" << std::endl;
  }
  else
  {
    std::cout << "Absolute" << std::endl;
  }
  if (fn.isDirectory())
  {
    std::cout << "Dir" << std::endl;
  }
  else
  {
    std::cout << "Not Dir" << std::endl;
  }
}

void testNoExt()
{
  AFilename f("c:/somedir/foo.txt");
  AString noext;
  f.emitFilenameNoExt(noext);
  std::cout << noext << std::endl;
}

void testCompare()
{
  AFilename left ("./aos_root/static/classified/index.html");
  AFilename right("./aos_root/static/rte/html2xhtml.js");

  //AString left ("index.html");
  //AString right("html2xhtml.js");

  if (left < right)
    std::cout << "left < right" << std::endl;
  if (right < left)
    std::cout << "right < left" << std::endl;
}

int main()
{
  //testParse();
  //testFormatting();
  //testCopy();
  //testJoin();
  //testRelativePath();
  //testExt();
  //testNoExt();
  testCompare();

  return 0;
}