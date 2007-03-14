#ifndef _templateListOfPtrs_HPP_
#define _templateListOfPtrs_HPP_

#include "apiABase.hpp"

/**
std::list wrapper that will delete all of its contents when done

Usage:
Before:  
  std::vector<AString *> myVector;
  myVector.push_back(new AString("foo"));
  ....
  //later need to delete the object
         
Now:
  AListOfPtrs<AString> myList;
  myVector._list.push_back(new AString("foo"));   // this class will delete this object in dtor
**/

template<class T>
class AListOfPtrs
{
public:
  typedef std::list<T*> TYPEDEF;
  TYPEDEF _list;

  /**
  Will call delete on all members 
  **/
  ~AListOfPtrs()
  {
    TYPEDEF::iterator it = _list.begin();
    while (it != _list.end())
    {
      delete *it;
      ++it;
    }
  }
};

#endif // _templateListOfPtrs_HPP_

