#include "pchUnitTests.hpp"
#include "ARopeDeque.hpp"
#include "AFile_AString.hpp"

void testGeneral(int& iRet)
{
  AString str;
  ARopeDeque rd(8);
  ASSERT_UNIT_TEST(rd.isEmpty(), "ARopeDeque::isEmpty", "0", iRet);

  //a_  "{[_0123456789_]abcdefghijklmnopqrstuvwxyz}"
  rd.pushBack("abcd");
  rd.pushBack("efghijklmnop");
  rd.pushBack("qrstu");
  rd.pushFront("_]");
  rd.pushFront("0123456789");
  rd.pushFront("[_");
  rd.pushBack("vwxyz}");
  rd.pushFront("{");
  ASSERT_UNIT_TEST(!rd.isEmpty(), "ARopeDeque::isEmpty", "1", iRet);

  rd.popFront(str, 2);
  ASSERT_UNIT_TEST(str.equals("{["), "ARopeDeque::getFront", "", iRet);

  str.clear();
  rd.popBack(str, 9);
  ASSERT_UNIT_TEST(str.equals("stuvwxyz}"), "ARopeDeque::getBack", "", iRet);

  str.clear();
  rd.emit(str);
  ASSERT_UNIT_TEST(str.equals("_0123456789_]abcdefghijklmnopqr"), "ARopeDeque::emit", "", iRet);
  ASSERT_UNIT_TEST(rd.getSize() == 31, "RopeDeque::getSize", "", iRet);

  rd.clear();
  ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::getSize after clear", "", iRet);
  ASSERT_UNIT_TEST(rd.isEmpty(), "ARopeDeque::isEmpty", "2", iRet);

  AFile_AString file;

  rd.pushBack("To witness such a show of power overthown.");
  ASSERT_UNIT_TEST(!rd.isEmpty(), "ARopeDeque::isEmpty", "3", iRet);
  rd.pushFront("Existing on damnation's edge the priest had never known.\n");
  ASSERT_UNIT_TEST(!rd.isEmpty(), "ARopeDeque::isEmpty", "4", iRet);

  rd.toAFile(file);

  file.useAString().assign("\nAngels dying aimlessly still dying by the sword.\n");
  rd.fromAFile(file);
  rd.pushBack("Our legions killing all in sight to get the one called lord.");

  ASSERT_UNIT_TEST(rd.toAString().equals("Existing on damnation's edge the priest had never known.\nTo witness such a show of power overthown.\nAngels dying aimlessly still dying by the sword.\nOur legions killing all in sight to get the one called lord."), "RopeDeque::toString()", "", iRet);
  ASSERT_UNIT_TEST(rd.getSize() == 209, "RopeDeque::getSize after file access", "", iRet);

  rd.clear(true);
  ASSERT_UNIT_TEST(rd.isEmpty(), "ARopeDeque::isEmpty", "5", iRet);
}

void testPopBack(int& iRet)
{
  ARopeDeque rd(5);
  rd.pushBack("4567890ABC");
  rd.pushFront("0123");
  //a_"0123 4567 890A BC"

  AString str("A=");
  rd.popBack(str, 5);  //a_"90ABC"
  ASSERT_UNIT_TEST(str.equals("A=90ABC"), "RopeDeque::popBack pop with str contents", "", iRet);

  str.clear();
  rd.popBack(str, 7);   //a_"2345678"
  ASSERT_UNIT_TEST(str.equals("2345678"), "RopeDeque::popBack pop with more than block empty back", "", iRet);

  str.clear();
  rd.popBack(str, 2);   //a_"01"
  ASSERT_UNIT_TEST(str.equals("01"), "RopeDeque::popBack leftovers in front", "", iRet);

  rd.pushFront("0123456789");
  str.clear();
  rd.popBack(str, 12);
  ASSERT_UNIT_TEST(str.equals("0123456789"), "RopeDeque::popBack push front pop more than in buffer", "", iRet);

  rd.pushBack("0123456789");
  str.clear();
  rd.popBack(str, 12);
  ASSERT_UNIT_TEST(str.equals("0123456789"), "RopeDeque::popBack pushBack less than in buffer", "", iRet);

  rd.pushFront("0123456789");
  rd.pushBack("abcdef");
  str.clear();
  rd.popBack(str, 11);
  ASSERT_UNIT_TEST(str.equals("56789abcdef"), "RopeDeque::popBack pushBack pushFront pop", "", iRet);
  ASSERT_UNIT_TEST(rd.getSize() == 5, "RopeDeque::popBack size inconsistency", "", iRet);

  str.clear();
  rd.popBack(str, 1);
  ASSERT_UNIT_TEST(str.equals("4"), "RopeDeque::popBack 2 char in back", "", iRet);

  str.clear();
  rd.popBack(str, 1);
  ASSERT_UNIT_TEST(str.equals("3"), "RopeDeque::popBack 1 char in back", "", iRet);

  str.clear();
  rd.popBack(str, 1);
  ASSERT_UNIT_TEST(str.equals("2"), "RopeDeque::popBack 1 char in back now empty", "", iRet);

  str.clear();
  rd.popBack(str, 1);
  ASSERT_UNIT_TEST(str.equals("1"), "RopeDeque::popBack 1 char in front", "", iRet);

  str.clear();
  rd.popBack(str, 1);
  ASSERT_UNIT_TEST(str.equals("0"), "RopeDeque::popBack 1 char in front now empty", "", iRet);
  ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popBack size should be 0", "", iRet);
}

void testPopFront(int& iRet)
{
  ARopeDeque rd(3);
  rd.pushBack("4567890ABC");
  rd.pushFront("0123");
  rd.pushBack("DEF");
  //a_"0123 4567 890A BCDE F"

  AString str;
  rd.popFront(str, 3);     //a_"012"
  ASSERT_UNIT_TEST(str.equals("012"), "RopeDeque::popFront 3char", "0", iRet);

  rd.popFront(str, 3);    //a_"012345"
  ASSERT_UNIT_TEST(str.equals("012345"), "RopeDeque::popFront 3char", "1", iRet);

  rd.popFront(str, 3);    //a_"012345678"
  ASSERT_UNIT_TEST(str.equals("012345678"), "RopeDeque::popFront 3char", "2", iRet);

  rd.popFront(str, 3);    //a_"012 345 678 9AB"
  ASSERT_UNIT_TEST(str.equals("01234567890A"), "RopeDeque::popFront 3char", "3", iRet);

  rd.popFront(str, 6);    //a_"0123456789ABCDEF"
  ASSERT_UNIT_TEST(str.equals("01234567890ABCDEF"), "RopeDeque::popFront 6char", "", iRet);
}

void testPeekUntil(int& iRet)
{
  ARopeDeque rd(7);
  AString str;
  u4 ret;
  rd.pushBack("IJKLMNOPQRSTUVWXYZ");

  rd.peekFrontUntil(str, "MNOPQR");
  ASSERT_UNIT_TEST(str.equals("IJKL"), "RopeDeque::peekUntil", "0", iRet);
  
  str.clear();
  rd.peekFrontUntil(str, "Y");
  ASSERT_UNIT_TEST(str.equals("IJKLMNOPQRSTUVWX"), "RopeDeque::peekUntil", "1", iRet);

  rd.pushFront("ABCDEFGH");
  str.clear();
  rd.peekFrontUntil(str, "Y");
  ASSERT_UNIT_TEST(str.equals("ABCDEFGHIJKLMNOPQRSTUVWX"), "RopeDeque::peekUntil", "2", iRet);
  str.clear();
  rd.peekFrontUntil(str, 'Y');
  ASSERT_UNIT_TEST(str.equals("ABCDEFGHIJKLMNOPQRSTUVWX"), "RopeDeque::peekUntil", "2a", iRet);

  str.clear();
  rd.peekFrontUntil(str, "CDEFGHI");
  ASSERT_UNIT_TEST(str.equals("AB"), "RopeDeque::peekUntil", "3", iRet);

  str.clear();
  rd.peekFrontUntil(str, "JKLM");
  ASSERT_UNIT_TEST(str.equals("ABCDEFGHI"), "RopeDeque::peekUntil", "4", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, 'J');
  ASSERT_UNIT_TEST(ret == 9, "RopeDeque::peekUntil", "5", iRet);
  ASSERT_UNIT_TEST(str.equals("ABCDEFGHI"), "RopeDeque::peekUntil", "5a", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, 'K', 20);
  ASSERT_UNIT_TEST(ret == 10, "RopeDeque::peekUntil", "6", iRet);
  ASSERT_UNIT_TEST(str.equals("ABCDEFGHIJ"), "RopeDeque::peekUntil", "6a", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, 'C', 2);
  ASSERT_UNIT_TEST(ret == 2, "RopeDeque::peekUntil", "7", iRet);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::peekUntil", "7a", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, 'I', 6);
  ASSERT_UNIT_TEST(ret == 6, "RopeDeque::peekUntil", "8", iRet);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::peekUntil", "8a", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, 'Y', 13);
  ASSERT_UNIT_TEST(ret == 13, "RopeDeque::peekUntil", "9", iRet);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::peekUntil", "9a", iRet);

  str.clear();
  ret = rd.peekFrontUntil(str, '1');
  ASSERT_UNIT_TEST(ret == AConstant::npos, "RopeDeque::peekUntil", "10", iRet);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::peekUntil", "10a", iRet);
}

void testRemoveUntil(int& iRet)
{
  u4 ret;
  AString str;
  {
    ARopeDeque rd(5);
    rd.pushBack("IJKLMNOPQRSTUVW");
    ret = rd.removeFrontUntil("NO", false);
    ASSERT_UNIT_TEST(ret == 5, "RopeDeque::removeFrontUntil", "0", iRet);
    
    rd.emit(str);
    ASSERT_UNIT_TEST(str.equals("NOPQRSTUVW"), "RopeDeque::removeFrontUntil", "1", iRet);

    ret = rd.removeFrontUntil("NO", true);
    ASSERT_UNIT_TEST(ret == 2, "RopeDeque::removeFrontUntil", "2", iRet);

    str.clear();
    rd.emit(str);
    ASSERT_UNIT_TEST(str.equals("PQRSTUVW"), "RopeDeque::removeFrontUntil", "3", iRet);

    rd.pushFront("ABCD");

    ret = rd.removeFrontUntil("UVW", true);
    ASSERT_UNIT_TEST(ret == 12, "RopeDeque::removeFrontUntil", "4", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::removeFrontUntil", "5", iRet);
  }

  {
    ARopeDeque rd(5);
    rd.pushBack("IJKLMNOPQRSTUVW");

    ret = rd.removeFrontUntil('K', true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::removeFrontUntil", "6", iRet);

    ret = rd.removeFrontUntil('N', false);
    ASSERT_UNIT_TEST(ret == 2, "RopeDeque::removeFrontUntil", "7", iRet);

    ret = rd.removeFrontUntil('U', false, 3);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::removeFrontUntil", "8", iRet);

    ret = rd.removeFrontUntil('S', false, 20);
    ASSERT_UNIT_TEST(ret == 5, "RopeDeque::removeFrontUntil", "9", iRet);

    ret = rd.removeFrontUntil('S', false, 20);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::removeFrontUntil", "10", iRet);

    ret = rd.removeFrontUntil('S', true, 20);
    ASSERT_UNIT_TEST(ret == 1, "RopeDeque::removeFrontUntil", "11", iRet);

    ret = rd.removeFrontUntil('U', true, 20);
    ASSERT_UNIT_TEST(ret == 2, "RopeDeque::removeFrontUntil", "12", iRet);

    ret = rd.removeFrontUntil('1', false, 20);
    ASSERT_UNIT_TEST(ret == AConstant::npos, "RopeDeque::removeFrontUntil", "13", iRet);
  }
}

void testPopFrontUntil(int& iRet)
{
  u4 ret;

  AString str;
  {
    ARopeDeque rd(3);
    rd.pushFront("ABCDEF");
    rd.popFrontUntil(str, "DE", false, true);
    ASSERT_UNIT_TEST(str.equals("ABC"), "RopeDeque::popFrontUntil", "0", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "EF", true, false);
    ASSERT_UNIT_TEST(str.equals("DEF"), "RopeDeque::popFrontUntil", "1", iRet);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popFrontUntil", "1a", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "2", iRet);
  }

  {
    ARopeDeque rd(5);
    rd.pushFront("ABCDEFGHIJKLMNOP");
    str.clear();
    ret = rd.popFrontUntil(str, "NO", false, true);
    ASSERT_UNIT_TEST(ret == 13, "RopeDeque::popFrontUntil", "3", iRet);
    ASSERT_UNIT_TEST(str.equals("ABCDEFGHIJKLM"), "RopeDeque::popFrontUntil", "4", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "Z", true);
    ASSERT_UNIT_TEST(ret == AConstant::npos, "RopeDeque::popFrontUntil", "5", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "P", true, false);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popFrontUntil", "6", iRet);
    ASSERT_UNIT_TEST(str.equals("NOP"), "RopeDeque::popFrontUntil", "7", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "8", iRet);
  }

  {
    ARopeDeque rd(3);
    rd.pushBack("ABCDEFGH");
    str.clear();
    ret = rd.popFrontUntil(str, "B", false, true);
    ASSERT_UNIT_TEST(ret == 1, "RopeDeque::popFrontUntil", "9", iRet);
    ASSERT_UNIT_TEST(str.equals("A"), "RopeDeque::popFrontUntil", "10", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "H", true, false);
    ASSERT_UNIT_TEST(ret == 7, "RopeDeque::popFrontUntil", "11", iRet);
    ASSERT_UNIT_TEST(str.equals("BCDEFGH"), "RopeDeque::popFrontUntil", "12", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "13", iRet);

    rd.pushBack("6789");
    rd.pushFront("45");
    rd.pushFront("123");
    ASSERT_UNIT_TEST(rd.getSize() == 9, "RopeDeque::popFrontUntil", "14", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "9", true);
    ASSERT_UNIT_TEST(ret == 9, "RopeDeque::popFrontUntil", "15", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "16", iRet);
  }

  {
    ARopeDeque rd(2);
    rd.pushBack("-----!----!---!--!");
    str.clear();
    ret = rd.popFrontUntil(str, "-!", true, true);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popFrontUntil", "16", iRet);
    ASSERT_UNIT_TEST(str.equals("----"), "RopeDeque::popFrontUntil", "17", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "--!", true, false);
    ASSERT_UNIT_TEST(ret == 5, "RopeDeque::popFrontUntil", "18", iRet);
    ASSERT_UNIT_TEST(str.equals("----!"), "RopeDeque::popFrontUntil", "19", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "!", false);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popFrontUntil", "19", iRet);
    ASSERT_UNIT_TEST(str.equals("---"), "RopeDeque::popFrontUntil", "20", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, "-!", true);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popFrontUntil", "21", iRet);
    ASSERT_UNIT_TEST(str.equals("!--!"), "RopeDeque::popFrontUntil", "22", iRet);
  }

  //a_Test with single char
  {
    ARopeDeque rd(3);
    rd.pushBack("ABCDEFGH");
    str.clear();
    ret = rd.popFrontUntil(str, 'B', false, true);
    ASSERT_UNIT_TEST(ret == 1, "RopeDeque::popFrontUntil", "17", iRet);
    ASSERT_UNIT_TEST(str.equals("A"), "RopeDeque::popFrontUntil", "18", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, 'H', true, false);
    ASSERT_UNIT_TEST(ret == 7, "RopeDeque::popFrontUntil", "19", iRet);
    ASSERT_UNIT_TEST(str.equals("BCDEFGH"), "RopeDeque::popFrontUntil", "20", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "21", iRet);

    rd.pushBack("6789");
    rd.pushFront("45");
    rd.pushFront("123");
    ASSERT_UNIT_TEST(rd.getSize() == 9, "RopeDeque::popFrontUntil", "22", iRet);

    str.clear();
    ret = rd.popFrontUntil(str, '9', true);
    ASSERT_UNIT_TEST(ret == 9, "RopeDeque::popFrontUntil", "23", iRet);
    ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::popFrontUntil", "24", iRet);
  }
}

void testGetUntilOneOf(int& iRet)
{
  u4 ret = 0;

  ARopeDeque rd("it's south of heaven.", 4);
  rd.pushFront("On and on ");
  AString str;

  ret = rd.peekFrontUntilOneOf(str, " ");
  ASSERT_UNIT_TEST(ret == 2, "RopeDeque::peekFrontUntilOneOf", "0", iRet);
  ASSERT_UNIT_TEST(str.equals("On"), "RopeDeque::peekFrontUntilOneOf", "1", iRet);

  str.clear();
  ret = rd.peekFrontUntilOneOf(str, "'");
  ASSERT_UNIT_TEST(ret == 12, "RopeDeque::peekFrontUntilOneOf", "2", iRet);
  ASSERT_UNIT_TEST(str.equals("On and on it"), "RopeDeque::peekFrontUntilOneOf", "3", iRet);

  str.clear();
  ret = rd.peekFrontUntilOneOf(str, ".!/_");
  ASSERT_UNIT_TEST(ret == 30, "RopeDeque::peekFrontUntilOneOf", "4", iRet);
  ASSERT_UNIT_TEST(str.equals("On and on it's south of heaven"), "RopeDeque::peekFrontUntilOneOf", "5", iRet);

  str.clear();
  ret = rd.peekFrontUntilOneOf(str, AConstant::ASTRING_EMPTY);
  ASSERT_UNIT_TEST(ret == AConstant::npos, "RopeDeque::peekFrontUntilOneOf", "6", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, " ", false);
  ASSERT_UNIT_TEST(ret == 2, "RopeDeque::popFrontUntilOneOf", "0", iRet);
  ASSERT_UNIT_TEST(str.equals("On"), "RopeDeque::popFrontUntilOneOf", "1", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, "'", true, true);
  ASSERT_UNIT_TEST(ret == 10, "RopeDeque::popFrontUntilOneOf", "2", iRet);
  ASSERT_UNIT_TEST(str.equals(" and on it"), "RopeDeque::popFrontUntilOneOf", "3", iRet);
  
  str.clear();
  ret = rd.popFrontUntilOneOf(str, ".", false); 
  ASSERT_UNIT_TEST(ret == 17, "RopeDeque::popFrontUntilOneOf", "4", iRet);
  ASSERT_UNIT_TEST(str.equals("s south of heaven"), "RopeDeque::popFrontUntilOneOf", "5", iRet);
  ASSERT_UNIT_TEST(rd.getSize() == 1, "RopeDeque::popFrontUntilOneOf", "6", iRet);

  str.clear();
  rd.clear();
  rd.pushFront("delim\nnodelim");
  ret = rd.popFrontUntilOneOf(str, "\n", true, true); 
  ASSERT_UNIT_TEST(ret == 5, "RopeDeque::popFrontUntilOneOf", "7", iRet);
  ASSERT_UNIT_TEST(str.equals("delim"), "RopeDeque::popFrontUntilOneOf", "8", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, "\n");
  ASSERT_UNIT_TEST(ret == AConstant::npos, "RopeDeque::popFrontUntilOneOf", "9", iRet);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popFrontUntilOneOf", "10", iRet);
}

void testAccess(int& iRet)
{
  int ret;

  ARopeDeque rd("_test_", 4);
  AString str;

  rd.pushFront("This_is_a");
  rd.removeBack(7);
  rd.pushBack("not_a");
  rd.popBack(str, 2);
  ASSERT_UNIT_TEST(str.equals("_a"), "RopeDeque::testAccess", "0", iRet);

  rd.pushBack("_really_a_test!");
  
  str.clear();
  rd.popFront(str,5);
  ASSERT_UNIT_TEST(str.equals("This_"), "RopeDeque::testAccess", "1", iRet);
  
  rd.pushFront("This_or_that_");

  //a_"This_or_that_is_not_really_a_test!"
  str.clear();
  rd.peekBack(str, 5);
  ASSERT_UNIT_TEST(str.equals("test!"), "RopeDeque::testAccess", "2", iRet);

  str.clear();
  rd.peekFront(str, 13);
  ASSERT_UNIT_TEST(str.equals("This_or_that_"), "RopeDeque::testAccess", "3", iRet);

  str.clear();
  ret = rd.peekFrontUntilOneOf(str, "_");
  ASSERT_UNIT_TEST(ret == 4, "RopeDeque::testAccess", "4", iRet);
  ASSERT_UNIT_TEST(str.equals("This"), "RopeDeque::testAccess", "5", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, "_", false);
  ASSERT_UNIT_TEST(ret == 4, "RopeDeque::testAccess", "6", iRet);
  ASSERT_UNIT_TEST(str.equals("This"), "RopeDeque::testAccess", "7", iRet);

  str.clear();
  rd.popFrontUntilOneOf(str, "_", true, false);
  ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::testAccess", "8", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, "_", true, true);
  ASSERT_UNIT_TEST(ret == 2, "RopeDeque::testAccess", "9", iRet);
  ASSERT_UNIT_TEST(str.equals("or"), "RopeDeque::testAccess", "10", iRet);

  str.clear();
  ret = rd.popFrontUntilOneOf(str, "_", true, true);
  ASSERT_UNIT_TEST(ret == 4, "RopeDeque::testAccess", "11", iRet);
  ASSERT_UNIT_TEST(str.equals("that"), "RopeDeque::testAccess", "12", iRet);

  str.clear();
  rd.removeFrontUntilOneOf("_");
  rd.removeFrontUntilOneOf("_");
  rd.removeFrontUntilOneOf("_");
  rd.popFrontUntilOneOf(str, "_");
  ASSERT_UNIT_TEST(str.equals("a_"), "RopeDeque::testAccess", "13", iRet);

  str.clear();
  rd.emit(str);
  rd.clear();
  ASSERT_UNIT_TEST(str.equals("test!"), "RopeDeque::testAccess", "14", iRet);

  str.clear();
  rd.pushFront("Remove_all!!!");
  rd.emit(str);
  ASSERT_UNIT_TEST(str.equals("Remove_all!!!"), "RopeDeque::testAccess", "15", iRet);

  rd.clear();
  ASSERT_UNIT_TEST(rd.getSize() == 0, "RopeDeque::testAccess", "15", iRet);
}

void testPopFrontUntilOneOf(int& iRet)
{
  u4 ret;
  AString strSet(" ");
  
  //a_Front block
  {
    AString str;
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "0a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set"), "RopeDeque::popUntilOneOf", "0b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "0c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "0d", iRet);
  }

  {
    AString str;
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "1a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set"), "RopeDeque::popUntilOneOf", "1b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "1c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "1d", iRet);
  }

  {
    AString str;
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popUntilOneOf", "2a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set "), "RopeDeque::popUntilOneOf", "2b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popUntilOneOf", "2c", iRet);
    ASSERT_UNIT_TEST(str.equals("the "), "RopeDeque::popUntilOneOf", "2d", iRet);
  }

  {
    AString str;
    ARopeDeque rd(16);
    rd.pushFront("Set the controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "3a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set"), "RopeDeque::popUntilOneOf", "3b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "3c", iRet);
    ASSERT_UNIT_TEST(str.equals("the"), "RopeDeque::popUntilOneOf", "3d", iRet);
  }

  //a_Middle blocks
  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "4a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "4b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "4c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "4d", iRet);
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "5a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "5b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "5c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "5d", iRet);
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 34, "RopeDeque::popUntilOneOf", "6a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of "), "RopeDeque::popUntilOneOf", "6b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popUntilOneOf", "6c", iRet);
    ASSERT_UNIT_TEST(str.equals("the "), "RopeDeque::popUntilOneOf", "6d", iRet);
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushFront("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "7a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "7b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "7c", iRet);
    ASSERT_UNIT_TEST(str.equals("the"), "RopeDeque::popUntilOneOf", "7d", iRet);
  }

  //a_Back  block
  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "8a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "8b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, false);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "8c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "8d", iRet);
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "9a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "9b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, false, true);
    ASSERT_UNIT_TEST(ret == 0, "RopeDeque::popUntilOneOf", "9c", iRet);
    ASSERT_UNIT_TEST(str.isEmpty(), "RopeDeque::popUntilOneOf", "9d", iRet);
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 34, "RopeDeque::popUntilOneOf", "10a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of "), "RopeDeque::popUntilOneOf", "10b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, false);
    ASSERT_UNIT_TEST(ret == 4, "RopeDeque::popUntilOneOf", "10c", iRet);
    ASSERT_UNIT_TEST(str.equals("the "), "RopeDeque::popUntilOneOf", "10d", iRet);
    std::cout << std::endl;
  }

  {
    AString str;
    strSet.assign(" ");
    ARopeDeque rd(16);
    rd.pushBack("Set_the_controls_for_the_heart_of the sun");

    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 33, "RopeDeque::popUntilOneOf", "11a", iRet);
    ASSERT_UNIT_TEST(str.equals("Set_the_controls_for_the_heart_of"), "RopeDeque::popUntilOneOf", "11b", iRet);
    str.clear();
    ret = rd.popFrontUntilOneOf(str, strSet, true, true);
    ASSERT_UNIT_TEST(ret == 3, "RopeDeque::popUntilOneOf", "11c", iRet);
    ASSERT_UNIT_TEST(str.equals("the"), "RopeDeque::popUntilOneOf", "11d", iRet);
  }
}

int ut_ARopeDeque_General()
{
  std::cerr << "ut_ARopeDeque_General" << std::endl;

  int iRet = 0x0;

  testGeneral(iRet);
  std::cerr << std::endl;
  testPopBack(iRet);
  std::cerr << std::endl;
  testPopFront(iRet);
  std::cerr << std::endl;
  testPeekUntil(iRet);
  std::cerr << std::endl;
  testRemoveUntil(iRet);
  std::cerr << std::endl;
  testPopFrontUntil(iRet);
  std::cerr << std::endl;
  testGetUntilOneOf(iRet);
  std::cerr << std::endl;
  testAccess(iRet);
  std::cerr << std::endl;
  testPopFrontUntilOneOf(iRet);

  return iRet;
}