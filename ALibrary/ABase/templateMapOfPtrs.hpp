/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef _templateMapOfPtrs_HPP_
#define _templateMapOfPtrs_HPP_

#include "apiABase.hpp"

/*!
std::map wrapper that will delete all of its contents when done

Usage:
Before:
  std::map<int, AString> myMap;
  myMap[0] = AString("foo");

Now:
  AMapOfPtrs<int, AString> myMap;
  myMap[0] = new AString("foo");   // this class will delete this object in dtor
*/
template<class K, class T>
class AMapOfPtrs : public std::map<K, T*>
{
public:
  /*!
  Default ctor
  */
  AMapOfPtrs() {}

  /*!
  Will call delete on all members
  */
  virtual ~AMapOfPtrs()
  {
    try
    {
      for (typename std::map<K, T*>::iterator it = this->begin(); it != this->end(); ++it)
        delete (*it).second;
    } catch(...) {}
  }

private:
  /*!
  No default copy ctor or assignment
  Should be done explicitly to properly copy the contained pointers
  */
  AMapOfPtrs(const AMapOfPtrs&) {}
  AMapOfPtrs& operator=(const AMapOfPtrs&) { return *this; }
};

#endif // _templateMapOfPtrs_HPP_
