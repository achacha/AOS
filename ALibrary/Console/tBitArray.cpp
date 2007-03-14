#include "ABitArray.hpp"
#include <iostream>
#include <cassert>

void testAccess()
{
  ABitArray ar;
  ar.setSize(35);
  ar.set(24, 1);
  assert(ar.get(24));
  ar.set(33, 1);
  ar.set(33, 0);
  ar.setSize(65);
  ar.setSize(34);
  
  assert(!ar.get(16));

  for (u4 x = 0; x < ar.getSize(); ++x) {
    std::cout << x << "=" << (ar.get(x) ? "1" : "0") << ", ";
  }
}

void testCopy()
{
  ABitArray ar;
  ar.setSize(9);
  ar.set(1, 1);
  ar.set(3, 1);
  ar.set(5, 1);
  ar.set(7, 1);
  ar.set(8, 1);
  std::cout << "Original" << std::endl;
  u4 x;
  for (x = 0; x < ar.getSize(); ++x) {
    std::cout << x << "=" << (ar.get(x) ? "1" : "0") << ", ";
  }
  
  ABitArray arCopy(ar);
  
  std::cout << std::endl << "Copy" << std::endl;
  for (x = 0; x < arCopy.getSize(); ++x) {
    std::cout << x << "=" << (arCopy.get(x) ? "1" : "0") << ", ";
  }
}

void testSet()
{
  ABitArray ar;
  AString original("01100101010101010001010101001010101010101"), result;
  ar.set(original);
  ar.get(result);
  ar.debugDump(std::cout);
  std::cout << std::endl;

  if (original != result)
    std::cout << "NOT EQUAL" << std::endl;
  else
    std::cout << "EQUAL" << std::endl;
}

void testReset()
{
  ABitArray ar;
  ar.setSize(17);
  ar.reset(0);
  ar.debugDump(std::cout);
  std::cout << std::endl;
  ar.reset(1);
  ar.debugDump(std::cout);
  std::cout << std::endl;
}

void testSetGetHex()
{
  AString str;
  ABitArray ar;
  ar.set("12345678", ABitArray::Hexadecimal);
  ar.debugDump(std::cout);

  std::cout << std::endl;
  ar.get(str, ABitArray::Hexadecimal);
  std::cout << str << std::endl;
  ar.get(str, ABitArray::Binary);
  std::cout << str << std::endl;

  std::cout << std::endl;
  ar.set(0, 1);
  ar.set(2, 1);
  ar.get(str, ABitArray::Hexadecimal);
  std::cout << str << std::endl;
  ar.get(str, ABitArray::Binary);
  std::cout << str << std::endl;
  
}

void testConversionPartials()
{
  ABitArray ar;
  AString str;
  ar.set("3");
  ar.get(str, ABitArray::Binary);
  std::cout << str << std::endl;

  ar.set("7F");
  ar.get(str, ABitArray::Binary);
  std::cout << str << std::endl;

  ar.set("1FF");
  ar.get(str, ABitArray::Binary);
  std::cout << str << std::endl;

  ar.set("111111111", ABitArray::Binary);
  ar.get(str, ABitArray::Hexadecimal);
  std::cout << str << std::endl;
}

int main()
{
//  testAccess();
//  testCopy();
//  testSet();
//  testReset();
//  testSetGetHex();
  testConversionPartials();

  return 0x0;
}
