#include <ATextConverter.hpp>

int main()
{
  AString str;
  str = ATextConverter::encodeBase64("foo:bar");
  if (str != "Zm9vOmJhcg==")
  {
    std::cerr << "ATextConverter Base64 encode failure" << std::endl;
  }
  std::cout << "Correct encoding: " << str << std::endl;

  str = ATextConverter::decodeBase64("Zm9vOmJhcg==");
  if (ATextConverter::decodeBase64("Zm9vOmJhcg==") != "foo:bar")
  {
    std::cerr << "ATextConverter Base64 decode failure" << std::endl;
  }
  std::cout << "Correct decoding: " << str << std::endl;
  
  return 0x0;
}
