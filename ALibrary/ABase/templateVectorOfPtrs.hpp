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
  myVector.push_back(new AString("foo"));   // this class will delete this object in dtor
**/

template<class T>
class AVectorOfPtrs : public std::vector<T*>
{
public:
  /*!
  Default ctor
  */
  AVectorOfPtrs() {}

  /*!
  ctor with initial size
  */
  AVectorOfPtrs(size_t initialSize) : std::vector<T*>(initialSize) {}

  /**
  Will call delete on all members 
  **/
  ~AVectorOfPtrs()
  {
    try
    {
      for (iterator it = begin(); it != end(); ++it)
        delete *it;
    } catch(...) {}
  }

private:
  /*!
  No default copy ctor or assignment
  Should be done explicitly to properly copy the contained pointers
  */
  AVectorOfPtrs(const AVectorOfPtrs&) {}
  AVectorOfPtrs& operator=(const AVectorOfPtrs&) { return *this }
};

#endif // _templateVectorOfPtrs_HPP_
