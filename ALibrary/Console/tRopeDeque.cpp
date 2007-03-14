#include "ARopeDeque.hpp"
#include "AFile_AString.hpp"

void testBasic()
{
  ARopeDeque rd(8);

  rd.pushBack("abcd");
  rd.pushBack("efghijklmnop");
  rd.pushBack("qrtsu");
  rd.pushFront(" Z");
  rd.pushFront("0123456789");
  rd.pushFront("Z ");
  rd.pushBack("vwxyz");

//  rd.debugDump();

  std::cout << rd.toAString() << std::endl;
}

void testFileAccess()
{
  AFile_AString file;
  ARopeDeque rd(16);

  rd.pushBack("To witness such a show of power overthown.");
  rd.pushFront("Existing on damnation's edge the priest had never known.\n");

  rd.toAFile(file);
  std::cout << file.useAString() << std::endl;

  file.useAString().assign("\nAngels dying aimlessly still dying by the sword.\nOur legions killing all in sight to get the one called lord.");
  rd.fromAFile(file);

  std::cout << rd.toAString() << std::endl;
}

void testFileReadWrite()
{
  AFile_AString strfile;
  AString str;

  strfile.write("Test\n");
  strfile.setWritePos(4);
  strfile.writeLine("Line 000");
  strfile.setReadPos(4);
  
  
  
  strfile.readUntilOneOf(str);
  if (str.compare("Line"))
  {
    std::cerr << "Read until error 1" << std::endl;
  } else std::cerr << "." << std::flush;

  str.clear();
  strfile.readUntilOneOf(str);
  if (str.compare("000"))
  {
    std::cerr << "Read until, EOF found" << std::endl;
  } else std::cerr << "." << std::flush;

  str.clear();
  strfile.readUntilEOF(str);
  if (str.compare("000"))
  {
    std::cerr << "Read untilEOF" << std::endl;
  } else std::cerr << "." << std::flush;
}

void testPopBack()
{
  ARopeDeque rd(4);
  rd.pushBack("4567890ABC");
  rd.pushFront("0123");
  //a_"0123 4567 890A BC"

  AString str("A=");
  rd.popBack(str, 5);  //a_"90ABC"
  std::cout << str << std::endl;

  str.clear();
  rd.popBack(str, 7);   //a_"2345678"
  std::cout << str << std::endl;

  str.clear();
  rd.popBack(str, 2);   //a_"01"
  std::cout << str << std::endl;

  rd.pushFront("0123456789");
  str.clear();
  rd.popBack(str, 12);
  std::cout << str << std::endl;
//  rd.debugDump();

  rd.pushBack("0123456789");
  str.clear();
  rd.popBack(str, 12);
  std::cout << str << std::endl;

  rd.pushFront("0123456789");
  rd.pushBack("abcdef");
  str.clear();
  rd.popBack(str, 12);
  std::cout << str << std::endl;

  str.clear();
  rd.popBack(str, 1);
  std::cout << str << std::endl;
//  rd.debugDump();

  str.clear();
  rd.popBack(str, 1);
  std::cout << str << std::endl;
//  rd.debugDump();

  str.clear();
  rd.popBack(str, 1);
  std::cout << str << std::endl;
//  rd.debugDump();

  str.clear();
  rd.popBack(str, 1);
  std::cout << str << std::endl;
//  rd.debugDump();
}

void testPopFront()
{
  ARopeDeque rd(4);
  rd.pushBack("4567890ABC");
  rd.pushFront("0123");
  rd.pushBack("DEF");
  //a_"0123 4567 890A BCDE F"

  AString str;
  rd.popFront(str, 3);     //a_"012"
  std::cout << str << std::endl;

  rd.popFront(str, 3);    //a_"012345"
  std::cout << str << std::endl;

  rd.popFront(str, 3);    //a_"012345678"
  std::cout << str << std::endl;

  rd.popFront(str, 3);    //a_"0123456789AB"
  std::cout << str << std::endl;

  rd.popFront(str, 6);    //a_"0123456789ABCDEF"
  std::cout << str << std::endl;

  rd.clear();
}

void testPushPop()
{
  AString str;
  ARopeDeque rd(8);

  rd.pushBack("abcd");
  rd.pushBack("efghijklmnop");
  rd.pushBack("qrtsu");
  rd.pushFront("_]");
  rd.pushFront("0123456789");
  rd.pushFront("[_");
  rd.pushBack("vwxyz}");
  rd.pushFront("{");

  //a_  "{[_0123456789_]abcdefghijklmnopqrstuvwxyz}"
//  rd.debugDump();
  std::cout << rd.toAString() << std::endl;

  rd.popFront(str, 2);
  std::cout << str << std::endl;
}

void testPeekUntil()
{
  ARopeDeque rd("it's south of heaven.", 4);
  rd.pushFront("On and on ");
  AString str;

  if (rd.peekFrontUntilOneOf(str, "."))
    std::cout << str << std::endl;
  else
    std::cout << "Not found!" << std::endl;
}

void testGetUntil()
{
  ARopeDeque rd("it's south of heaven.", 4);
  rd.pushFront("On and on ");
  AString str;

  if (rd.popFrontUntilOneOf(str, " "))
    std::cout << str << std::endl;
  else
    std::cout << "Not found!" << std::endl;
//  rd.debugDump();

  if (rd.popFrontUntilOneOf(str, "'"))
    std::cout << str << std::endl;
  else
    std::cout << "Not found!" << std::endl;
//  rd.debugDump();


  if (rd.popFrontUntilOneOf(str, "."))
    std::cout << str << std::endl;
  else
    std::cout << "Not found!" << std::endl;

//  rd.debugDump();
}

void testAccess()
{
  ARopeDeque rd("_test_", 4);
  AString str;

  rd.pushFront("This_is_a");
  rd.removeBack(7);
  rd.pushBack("not_a");
  rd.popBack(str, 2);

  if (str != "_a") std::cout << "Problem 000" << std::endl;

  rd.pushBack("_really_a_test!");
  
  str.clear();
  rd.popFront(str,5);
  if (str != "This_") std::cout << "Problem 001" << std::endl;
  
  rd.pushFront("This_or_that_");
//  rd.debugDump();

  //a_"This_or_that_is_not_really_a_test!"
  str.clear();
  rd.peekBack(str, 5);
  if (str != "test!") std::cout << "Problem 002" << std::endl;

  str.clear();
  rd.peekFront(str, 13);
  if (str != "This_or_that_") std::cout << "Problem 003" << std::endl;

  str.clear();
  rd.peekFrontUntilOneOf(str, "_");
  if (str != "This") std::cout << "Problem 004" << std::endl;

  str.clear();
  rd.popFrontUntilOneOf(str, "_", false);
  if (str != "This") std::cout << "Problem 005" << std::endl;

  str.clear();
  rd.popFrontUntilOneOf(str, "_");
  if (!str.isEmpty()) std::cout << "Problem 006" << std::endl;

  str.clear();
  rd.popFrontUntilOneOf(str, "_");
  if (str != "or") std::cout << "Problem 007" << std::endl;

  str.clear();
  rd.popFrontUntilOneOf(str, "_");
  if (str != "that") std::cout << "Problem 008" << std::endl;

  str.clear();
  rd.removeFrontUntilOneOf("_");
  rd.removeFrontUntilOneOf("_");
  rd.removeFrontUntilOneOf("_");
  rd.popFrontUntilOneOf(str, "_");
  if (str != "a") std::cout << "Problem 009" << std::endl;

  str.clear();
  rd.emit(str);
  rd.clear();
  if (str != "test!") std::cout << "Problem 010" << std::endl;

  str.clear();
  rd.pushFront("Remove_all!!!");
  rd.emit(str);
  if (str != "Remove_all!!!") std::cout << "Problem 011" << std::endl;

  rd.clear();
  if (rd.getSize() != 0) std::cout << "Problem 012" << std::endl;
}

void testPopUntilOneOf()
{
  u4 ret;
  
  //a_Front block
  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  //a_Middle blocks
  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  //a_Back  block
  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "0 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 0 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }

  {
    AString str;
    AString strSet(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    std::cout << "1 1 -> ret=" << ret << " str='" << str << "'" << std::endl;
    std::cout << std::endl;
  }
}

void testTrickySearch()
{
  AString str;
  ARopeDeque rd(4);
  rd.pushFront("----1234");
  rd.peekFrontUntil(str, "--12");

  std::cout << str << std::endl;
}

void testAbsoluteFind()
{
  AString str;
  ARopeDeque rd(4);
  rd.pushFront("----1234567890---1234567890--123-12");
  u4 pos = 0;
  while (AConstant::npos != (pos = rd.find('1', pos)))
  {
    std::cout << "pos=" << pos << ", " << std::endl;
    ++pos;
  }
}

void testAbsoluteCompare()
{
  AString str;
  ARopeDeque rd(4);
  rd.pushFront("----1234567890---1234567890--123-12");
  u4 pos = 0;
  pos = rd.compare(AString("1234"), 4);
  std::cout << "pos=" << pos << std::endl;

  pos = rd.compare(AString("-14"), 3);
  std::cout << "pos=" << pos << std::endl;

  pos = rd.compare(AString("1"), 1);
  std::cout << "pos=" << pos << std::endl;
}

void testPopFrontUntil()
{
  AString str;
  ARopeDeque rd(4);
  rd.pushFront("----1234567890---1234567890--123-12");
  u4 pos = 0;
  pos = rd.popFrontUntil(str, "--1", true, true);
  std::cout << "pos=" << pos << " str=" << str << std::endl;
}

void testPeekUntilWithLimit()
{
  AString target;
  ARopeDeque rd(4);
  rd.pushFront("0123456");
  rd.pushBack("789a");
//  rd.debugDump();

  std::cout << "Find 2 no limit return:" << rd.peekFrontUntil(target, '2') << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Find 2 with 1   return:" << rd.peekFrontUntil(target, '2', 1) << "  target=" << target << std::endl;

  target.clear();
  std::cout << "Find 6 no limit return:" << rd.peekFrontUntil(target, '6') << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Find 6 with 5   return:" << rd.peekFrontUntil(target, '6', 5) << "  target=" << target << std::endl;

  target.clear();
  std::cout << "Find 7 no limit return:" << rd.peekFrontUntil(target, '7') << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Find 7 with 5   return:" << rd.peekFrontUntil(target, '7', 5) << "  target=" << target << std::endl;

  target.clear();
  std::cout << "Find a no limit return:" << rd.peekFrontUntil(target, 'a') << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Find a with 9   return:" << rd.peekFrontUntil(target, 'a', 9) << "  target=" << target << std::endl;
}

void testPopUntilWithLimit()
{
  AString target;
  ARopeDeque rd(4);
  rd.pushFront("0123456");
  rd.pushBack("789a");

  std::cout << "Pop '2' with 4 max: " << rd.popFrontUntil(target, '2', false, true, 4) << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Pop '4' with 2 max: " << rd.popFrontUntil(target, '4', false, true, 2) << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Pop '4' with 10 max: " << rd.popFrontUntil(target, '4', true, false, 10) << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Pop '5' with no max: " << rd.popFrontUntil(target, '5', true, false) << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Pop '6' with 10 max: " << rd.popFrontUntil(target, '6', true, false, 10) << "  target=" << target << std::endl;
  target.clear();
  std::cout << "Pop 'b' with 10 max: " << rd.popFrontUntil(target, 'b', true, false, 10) << "  target=" << target << std::endl;
}

void testPutBack()
{
  ARopeDeque x(ASWNL("[this_is_the_inside_string]"), 4);
  ARopeDeque y(ASWNL("{outer_string_for_append_and_prepend}"), 3);

  x.pushBack(y);
  x.pushFront(y);
  x.pushFront(y);
  x.pushBack(y);
  
  std::cout << x << std::endl;
}

void testFindPattern()
{
  {
    ARopeDeque rd;

    size_t pos;
    if (AConstant::npos == (pos = rd.find(ASW("\r\n\r\n", 4))))
    {
      std::cout << "Not found" << std::endl;
    }
    else
    {
      std::cout << "Found at " << pos << std::endl;
    }
  }

  //a_Result should be 30
  {
    ARopeDeque rd;
    rd.pushBack("GET / HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\na=1&b=2");
//    rd.debugDump();

    size_t pos;
    if (AConstant::npos == (pos = rd.find(ASW("\r\n\r\n", 4))))
    {
      std::cout << "Not found?" << std::endl;
    }
    else
    {
      std::cout << "Found at " << pos << std::endl;
    }

    rd.clear();
    rd.pushFront("GET / HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\na=1&b=2");
    if (AConstant::npos == (pos = rd.find(ASW("\r\n\r\n", 4))))
    {
      std::cout << "Not found?" << std::endl;
    }
    else
    {
      std::cout << "Found at " << pos << std::endl;
    }
  }

  {
    ARopeDeque rd(3);
    rd.pushBack("GET / HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\na=1&b=2");
//    rd.debugDump();

    size_t pos;
    if (AConstant::npos == (pos = rd.find(ASW("\r\n\r\n", 4))))
    {
      std::cout << "Not found?" << std::endl;
    }
    else
    {
      std::cout << "Found at " << pos << std::endl;
    }

    rd.clear();
    rd.pushFront("GET / HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\na=1&b=2");
    if (AConstant::npos == (pos = rd.find(ASW("\r\n\r\n", 4))))
    {
      std::cout << "Not found?" << std::endl;
    }
    else
    {
      std::cout << "Found at " << pos << std::endl;
    }
  }
}

int main()
{
//  testBasic();
//  testFileAccess();
//  testFileReadWrite();
//  testPopBack();
//  testPopFront();
//  testPushPop();
//  testPeekUntil();
//  testGetUntil();
//  testAccess();
//  testPopUntilOneOf();
//  testTrickySearch();
//  testAbsoluteFind();
//  testAbsoluteCompare();
//  testPopFrontUntil();
//  testPeekUntilWithLimit();
//  testPopUntilWithLimit();
//  testPutBack();
  testFindPattern();

  return 0;
}
