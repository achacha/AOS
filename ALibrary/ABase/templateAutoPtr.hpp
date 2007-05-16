#ifndef INCLUDED__templateAutoPtr_HPP__
#define INCLUDED__templateAutoPtr_HPP__

#include "apiABase.hpp"

// Automatic Scope-Lifetime pointer wrapper templates
//---------------------------------------------------
// AAutoPtr           - for single object types
// AAutoArrayPtr      - for an array of object
// AAutoBasicArrayPtr - for an array of basic types

// Example AutoPtr:
//   AAutoPtr<MyObject> pOb(new MyObject);    //Single object deleted when leaves scope
//
// Equals to:
//   MyObject* pOb = new MyObject;
//   delete pOb;


// Example AutoPtr:
//   AAutoArrayPtr<MyObject> pOb[3];           //Array of objects deleted when leaves scope
//   pOb[0] = new MyObject;
//   pOb[1] = new MyObject;
//   pOb[2] = new MyObject;
//
// Equals to:
//   MyObject* pOb = new MyObject;
//   pOb[0] = new MyObject;
//   pOb[1] = new MyObject;
//   pOb[2] = new MyObject;
//   delete[] pOb;

// Example AutoPtr:
//   AAutoBasicArrayPtr<char> pcBuf(new char[128]);   //Array of basic types deleted when leaves scope
//   strcpy(pcBuf, "HelloWorld");
//
// Equals to:
//   char* pcBuf = new char[128];
//   strcpy(pcBuf, "HelloWorld");
//   delete[] pcBuf;
//

// Using AutoArrayPtr<> as a dynamic array of AAutoPtr<>
// This will automatically deallocate the array and all of its members when it leaves scope
//   int COUNT = 6;
//   AAutoArrayPtr< AAutoPtr<MyObject> > objectArray;
//   objectArray = new AutoStatPtr[COUNT];
//   for(int i=0; i < COUNT; ++i)
//     objectArray[i] = new MyObject;
//   ...
//   objectArray[i]->callSomething();
//
// Equals to:
//   int COUNT = 6;
//   MyObject *objectArray = new MyObject[COUNT];
//   for(int i=0; i < COUNT; ++i)
//     objectArray[i] = new MyObject;
//   ...
//   objectArray[i]->callSomething();
//   ...
//   for(int i=0; i < COUNT; ++i)
//     delete objectArray[i];
//   delete []objectArray;

//a_Pointer to an object (NOT to be used with arrays, see below)
template <class T>
class AAutoPtr
{
public:
	explicit AAutoPtr(T* pointer = NULL) : m__Pointer(pointer), mbool__Owns(true) {}
	~AAutoPtr()
  { 
    if (mbool__Owns) delete m__Pointer;
  }
 
  /*!
  Assign to a pointer and take ownership
  */
  AAutoPtr& operator=(T* pointer)
  {
    reset(pointer);
    return *this;
  }

  /*!
  Assign to pointer and take ownership by default
  If previous pointer is owned, it is deleted
  */
  void reset(T* pointer = NULL, bool owns = true)
  {
    if (mbool__Owns) delete m__Pointer;
    m__Pointer = pointer;
    mbool__Owns = owns;
  }

  //a_Cast and access
        T* operator->()       { return m__Pointer; }
  const T* operator->() const { return m__Pointer; }
  operator       T*  ()        { return m__Pointer; }
  operator const T*  () const  { return m__Pointer; }
  
  //a_Pointer access and tests
  T* get()                    { return m__Pointer; }
  const T* const get() const  { return m__Pointer; }
  bool isNull()               { return (m__Pointer == NULL); }

  //a_Change of ownership
  void setOwnership(bool boolOwns) { mbool__Owns = boolOwns; }

private:
	T*   m__Pointer;
	bool mbool__Owns;
};


//a_AAutoArrayPtr class ABASE_API is meant for pointers to arrays of classes
template <class T>
class AAutoArrayPtr
{
public:
	explicit AAutoArrayPtr(T* pointer = NULL) : m__Pointer(pointer), mbool__Owns(true) {}
	~AAutoArrayPtr()
  { 
    if (mbool__Owns) delete []m__Pointer;
  }

	//a_Set
  AAutoArrayPtr& operator=(T* pointer)
  { 
    reset(pointer);
    return *this;
  }
  void reset(T* pointer = NULL)
  { 
    if (mbool__Owns) delete []m__Pointer;
    m__Pointer = pointer;
    mbool__Owns = true;
  }
	
  //a_Cast and access
	operator T* ()              {return m__Pointer;}
  T* operator->()             {return m__Pointer;}
  operator const T* ()  const { return m__Pointer; }
	const T* operator->() const { return m__Pointer; }

  T* get()                    { return m__Pointer; }
  const T* const get() const  { return m__Pointer; }

	//a_Array access operators
  T &operator [](int iOffset) { return m__Pointer[iOffset]; }

	//a_Change of ownership
  void setOwnership(bool boolOwns) {mbool__Owns=boolOwns;}

private:
	T*   m__Pointer;
	bool mbool__Owns;
};


//a_AAutoBasicArrayPtr class is meant for pointers to arrays of basic types (ie. char *)
template <class T>
class AAutoBasicArrayPtr
{
public:
  explicit AAutoBasicArrayPtr(T* pointer = NULL) : m__Pointer(pointer), mbool__Owns(true) {}
  ~AAutoBasicArrayPtr()
  { 
    if (mbool__Owns) delete []m__Pointer;
  }
 
  AAutoBasicArrayPtr& operator=(T* pointer)
  {
    reset(pointer);
    return *this;
  }
  void reset(T* pointer = NULL)
  {
    if (mbool__Owns) delete []m__Pointer;
    m__Pointer = pointer; 
    mbool__Owns = true;
  }

  //a_Access
  operator       T* ()       { return m__Pointer; }
  operator const T* () const { return m__Pointer; }

  T* get()                   { return m__Pointer; }
  const T* const get() const  { return m__Pointer; }

  T& operator *()            { return *m__Pointer; }

  //a_Array access operators
  T& operator [](int iOffset) { return m__Pointer[iOffset]; }

	//a_Change of ownership
  void setOwnership(bool boolOwns) {mbool__Owns=boolOwns;}

private:
  T*   m__Pointer;
  bool mbool__Owns;
};

#endif

