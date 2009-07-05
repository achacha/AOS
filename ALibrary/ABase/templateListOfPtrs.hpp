/*
Written by Alex Chachanashvili

$Id$
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
  myList.use().push_back(new AString("foo"));   // this class will delete this object in dtor
*/
template<typename T>
class AListOfPtrs : public ABase
{
public:
  //! Type
  typedef std::list<T*> TYPE;

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
      for (TYPE::iterator it = m_Container.begin(); it != m_Container.end(); ++it)
        delete *it;
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
  AListOfPtrs(const AListOfPtrs&) {}
  AListOfPtrs& operator=(const AListOfPtrs&) { return *this; }
};

#endif // _templateListOfPtrs_HPP_

