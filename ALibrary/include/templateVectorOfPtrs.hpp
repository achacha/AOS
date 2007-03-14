#ifndef INCLUDED__templateVectorOfPtrs_HPP__
#define INCLUDED__templateVectorOfPtrs_HPP__

#include "apiABase.hpp"

/**
std::vector wrapper that will delete all of its contents when done

Usage:
Before:  
  std::vector<AString *> myVector;
  myVector.push_back(new AString("foo"));
  ...
  //later need to delete the content
         
Now:
  AVectorOfPtrs<AString> myVector;
  myVector._vector.push_back(new AString("foo"));   // this class will delete this object in dtor
**/

template<class T>
class AVectorOfPtrs
{
public:
  typedef std::vector<T*> TYPEDEF;
  TYPEDEF _vector;

  /**
  Will call delete on all members 
  **/
  ~AVectorOfPtrs()
  {
    TYPEDEF::iterator it = _vector.begin();
    while (it != _vector.end())
    {
      delete *it;
      ++it;
    }
  }
};

#endif // _templateVectorOfPtrs_HPP_
