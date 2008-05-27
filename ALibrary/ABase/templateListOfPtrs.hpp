/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef _templateListOfPtrs_HPP_
#define _templateListOfPtrs_HPP_

#include "apiABase.hpp"

/*!
std::list wrapper that will delete all of its contents when done

Usage:
Before:  
  std::vector<AString *> myVector;
  myVector.push_back(new AString("foo"));
  ....
  //later need to delete the object
         
Now:
  AListOfPtrs<AString> myList;
  myVector.push_back(new AString("foo"));   // this class will delete this object in dtor
*/
template<class T>
class AListOfPtrs : public std::list<T*>
{
public:
  /*!
  default ctor
  */
  AListOfPtrs() {}

  /*!
  Will call delete on all members 
  */
  ~AListOfPtrs()
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
  AListOfPtrs(const AListOfPtrs&) {}
  AListOfPtrs& operator=(const AListOfPtrs&) { return *this }
};

#endif // _templateListOfPtrs_HPP_

