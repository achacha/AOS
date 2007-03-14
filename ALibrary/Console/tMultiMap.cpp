#pragma warning(disable:4786)

#include <sgi/map>
#include <typedefContainers.hpp>
#include <AString.hpp>

int main()
{
  MMAP_AString_AString mm;
  
  mm.insert(MMAP_AString_AString::value_type("name", "0"));
  mm.insert(MMAP_AString_AString::value_type("name", "1"));
  mm.insert(MMAP_AString_AString::value_type("name", "2"));

  MMAP_AString_AString::iterator it = mm.find("name");
  while (it != mm.end())
  {
    cout << (*it).first.c_str() << "=" << (*it).second.c_str() << endl;
    it++;
  }
  
  return 0x0;
}
