#include <ATextGenerator.hpp>

#undef main
int main()
{
  AString strTemplate("Number=%N%n%n%n Word=%L%s%l%l%l Count=%z Time=%t\r\n  RFC_Time=%T");
  
  int iX;
  for (iX = 0x0; iX < 0x8; ++iX)
    cout << ATextGenerator::generateFromTemplate(strTemplate).c_str() << endl;

  cout << endl;
  ATextGenerator::resetCounter();

  for (iX = 0x0; iX < 0x8; ++iX)
    cout << ATextGenerator::generateFromTemplate(strTemplate).c_str() << endl;

  return 0x0;
}
