#include <iostream>

class Serial
{
public:
  virtual int serialMethod()
  {
    return 0x0;
  }
};

class Base : public Serial
{
public:
  Base() {}
  virtual ~Base() {}

  virtual int baseMethod(bool b = true) = 0;
};

class Intermediate : public Base
{
public:
  virtual int interMethod(int i = 0) = 0;
};

class Derived : public Intermediate
{
public:
  virtual baseMethod(bool b = true)
  { 
    return 0x1;
  }
  virtual interMethod(int i = 0)
  { 
    return 0x2;
  }
};

int main()
{
  Derived d;

  std::cout << "serial =" << d.serialMethod() << std::endl;
  std::cout << "base   =" << d.baseMethod() << std::endl;
  std::cout << "inter  =" << d.interMethod() << std::endl;
  
  return 0x0;
}
