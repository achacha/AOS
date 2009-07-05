/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__templateVectorOfPtrs_HPP__
#define INCLUDED__templateVectorOfPtrs_HPP__

#include "apiABase.hpp"

/**
std::vector wrapper that will delete all of its contents when done

Usage:
Before:  
  std::vector<AString *> myVector;
  myVector.use().push_back(new AString("foo"));
  ...
  //later need to delete the content
         
Now:
  AVectorOfPtrs<AString> myVector;
  myVector.use().push_back(new AString("foo"));   // this class will delete this object in dtor
**/

template<class T>
class AVectorOfPtrs : public ABase
{
public:
  typedef std::vector<T*> TYPE;

public:
  /*!
  Default ctor
  */
  AVectorOfPtrs() {}

  /*!
  ctor with initial size
  */
  AVectorOfPtrs(size_t initialSize) : m_Conatiner(initialSize) {}

  /**
  Will call delete on all members 
  **/
  ~AVectorOfPtrs()
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
  AVectorOfPtrs(const AVectorOfPtrs&) {}
  AVectorOfPtrs& operator=(const AVectorOfPtrs&) { return *this }
};

#endif // _templateVectorOfPtrs_HPP_
