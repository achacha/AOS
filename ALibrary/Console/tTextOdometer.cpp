#include "ATextOdometer.hpp"

void testOdometer()
{
  ATextOdometer todo(2);
  register int iX;

  std::cout << "-------  09 to 12 ---------" << std::endl;
  todo.setInitial("09");
  for (iX = 0x0; iX < 0x4; ++iX, todo++)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }

  std::cout << "-------  98 to 01 ---------" << std::endl;
  todo.setInitial("98");
  for (iX = 0x0; iX < 0x4; ++iX, ++todo)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }

  std::cout << "-------  11 to 08 ---------" << std::endl;
  todo.setInitial("11");
  for (iX = 0x0; iX < 0x4; ++iX, --todo)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }

  std::cout << "-------  02 to 97 ---------" << std::endl;
  todo.setInitial("2");
  for (iX = 0x0; iX < 0x4; ++iX, todo--)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }

  std::cout << "-------  0000 to 9998 to 0001 ---------" << std::endl;
  todo.reset(0x4);
  for (iX = 0x0; iX < 0x2; ++iX, todo--)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }
  for (iX = 0x0; iX < 0x4; ++iX, todo++)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }
}

void testHexOdometer()
{
  ATextOdometer todo(0x4, "0123456789ABCDEF");
  int iX;

  std::cout << "-------  FFFE to 0001 ---------" << std::endl;
  todo.setInitial("FFFE");
  for (iX = 0x0; iX < 0x4; ++iX, todo++)
  {
    std::cout << todo.toString().c_str() << std::endl;
  }
}

void testRandomOdometer()
{
  ATextOdometer todo(0x8, "01");
  int iX;

  for (iX = 0x0; iX < 0x8; ++iX)
    std::cout << todo.setRandomInitial() << ", ";
  std::cout << std::endl;

  todo.setSubset("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  todo.reset(0x3);
  for (iX = 0x0; iX < 0x8; ++iX)
    std::cout << todo.setRandomInitial() << ", ";
  std::cout << std::endl;

}

#undef main
int main()
{
//  testOdometer();
//  testHexOdometer();
  testRandomOdometer();

  return 0x0;
}
