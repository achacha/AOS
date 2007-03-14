#include "ATextConverter.hpp"

void testShortString()
{
  AString str("\x32\x00\x33\x01\x34\x02\x35\x03", 0x8);
  std::cout << ATextConverter::convertStringToHexDump(str) << std::endl;
}

void testLongString()
{
AString str("\x32\x00\x33\x01\x34\x02\x35\x03\
\x36\x04\x37\x05\x38\x06\x39\x07\
\x3A\x08\x3B\x09\x3C\x0A\x3D\x0B\
\x3E\x0C\x3F\x0D\x40\x0E\x41\x0F", 0x20);
  std::cout << ATextConverter::convertStringToHexDump(str) << std::endl;
}

void testVeryLongString()
{
AString str(
"\x32\x00\x33\x01\x34\x02\x35\x03\
\x36\x04\x37\x05\x38\x06\x39\x07\
\x3A\x08\x3B\x09\x3C\x0A\x3D\x0B\
\x3E\x0C\x3F\x0D\x40\x0E\x41\x0F\
\x42\x10\x43\x11\x44\x12\x45\x13\
\x46\x14\x47\x15\x48\x16\x49\x17\
\x4A\x18\x4B\x19\x4C\x1A\x4D\x1B\
\x4E\x1C\x4F\x1D\x50\x1E\x51\x1F\
\x52\x20\x53\x21\x54\x22\x55\x23\
\x56\x24\x57\x25\x58\x26\x59\x27\
\x5A\x28\x5B\x29\x5C\x2A\x5D\x2B\
\x5E\x2C\x5F\x2D\x60\x2E\x61\x2F\
\x62\x30\x63\x31\x64\x32\x65\x33\
\x66\x34\x67\x35\x68\x36\x69\x37\
\x6A\x38\x6B\x39\x6C\x3A\x6D\x3B\
\x6E\x3C\x6F\x3D\x70\x3E\x71\x3F\
\x72\x40\x73\x41\x74\x42\x75\x43\
\x76\x44\x77\x45\x78\x46\x79\x47\
\x7A\x48\x7B\x49\x7C\x4A\x7D\x4B\
\x7E\x4C\x7F\x4D\x80\x4E\x81\x4F\
\x82\x50\x83"
, 0xA3);
  std::cout << ATextConverter::convertStringToHexDump(str) << std::endl;
}

void testHEX()
{
  AString str = "\x41\x42\x43\x44";
  std::cout << "ASCII=" << str << std::endl;
  std::cout << "HEX  =" << ATextConverter::encodeHEX(str) << std::endl;
  std::cout << "ASCII=" << ATextConverter::decodeHEX(ATextConverter::encodeHEX(str)) << std::endl;
}

void testURL()
{
  AString str = "http://supernova.underpass.com/~achacha/VP.cgi?param=ssa";
  std::cout << "ASCII=" << str << std::endl;
  std::cout << "URL  =" << ATextConverter::encodeURL(str) << std::endl;
  std::cout << "ASCII=" << ATextConverter::decodeURL(ATextConverter::encodeURL(str)) << std::endl;
}


void test64()
{
  AString str("This is a test.");
  std::cout << "original : '" << ATextConverter::encodeURL(str) << "'" << std::endl;
  std::cout << "roundtrip: '" << ATextConverter::decodeURL(ATextConverter::encodeURL(str)) << "'" << std::endl;
}

int main()
{
  //testShortString();
  //testLongString();
  //testVeryLongString();
  //testHEX();
  test64();

  return 0x0;
}
