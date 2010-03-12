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
  myMap.use()[0] = AString("foo");

Now:
  AMapOfPtrs<int, AString> myMap;
  myMap.use()[0] = new AString("foo");   // this class will delete this object in dtor
*/
template<typename K, typename T>
class AMapOfPtrs : public ABase
{
public:
  typedef std::map<K, T*> TYPE;

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
      for (typename TYPE::iterator it = m_Container.begin(); it != m_Container.end(); ++it)
        delete (*it).second;
    } catch(...) {}
  }

  /*!
  Access the actual container
  */
  TYPE& use() { return m_Container; }

  /*!
  Access the actual constant container
  */
  const TYPE& get() const { return m_Container; }

protected:
  TYPE m_Container;

private:
  /*!
  No default copy ctor or assignment
  Should be done explicitly to properly copy the contained pointers
  */
  AMapOfPtrs(const AMapOfPtrs&) {}
  AMapOfPtrs& operator=(const AMapOfPtrs&) { return *this; }
};

#endif // _templateMapOfPtrs_HPP_
