#include <iostream.h>
#include <strstrea.h>
#include <AObject.hpp>
#include <AFileIOStream.hpp>

class MyObject : public AObject
{
  public:
    MyObject()
    {
      set(s_Count, AString::s_strZero);
      set(s_Name, AString::s_strEmpty);
    }

    void setName(const AString& str) { set(s_Name, str); }
    void setCount(int iNewCount) { set(s_Count, AString::fromInt(iNewCount)); }

    const AString& getName() const { return get(s_Name); }
    int getCount() const { return get(s_Count).toInt(); }
  
  private:
    static const AString s_Name;
    static const AString s_Count;
};
const AString MyObject::s_Name("name");
const AString MyObject::s_Count("count");

int main()
{
  strstream str;
  AFileIOStream screen(&cout), file(&str, &str);
  MyObject obj0, obj1;

  obj0.setCount(5);
  obj0.setName("This is my name");

//  obj0.debugDump(cerr, 0x0);
//  obj1.debugDump(cerr, 0x0);

  file << obj0;
  file >> obj1;

  if (obj0.compare(obj1))
  {
    cerr << "\r\nUnequal after serialization\r\n" << endl;
  }

  for (int iT = 0x0; iT < 512; ++iT)
  {
    obj1.setCount(iT);
    file << obj1;
    file >> obj0;
//    obj0.debugDump(cerr, 0x0);
//    obj1.debugDump(cerr, 0x0);
    if (obj0.compare(obj1))
    {
      cerr << "\r\nComparisson failed\r\n" << endl;
    }
  }


  return 0x0;
}
