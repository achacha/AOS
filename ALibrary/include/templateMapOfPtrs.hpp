#ifndef _templateMapOfPtrs_HPP_
#define _templateMapOfPtrs_HPP_

#include "apiABase.hpp"

/**
std::map wrapper that will delete all of its contents when done

Usage:
Before:  
  std::map<int, AString> myMap;
  myMap[0] = AString("foo");
         
Now:
  AMapOfPtrs<int, AString> myMap;
  myMap._map[0] = new AString("foo");   // this class will delete this object in dtor
**/

template<class K, class T>
class AMapOfPtrs
{
public:
  typedef std::map<K, T*> TYPEDEF;
  TYPEDEF _map;

  /**
  Will call delete on all members 
  **/
  ~AMapOfPtrs()
  {
    TYPEDEF::iterator it = _map.begin();
    while (it != _map.end())
    {
      delete (*it).second;
      ++it;
    }
  }
};

#endif // _templateMapOfPtrs_HPP_

