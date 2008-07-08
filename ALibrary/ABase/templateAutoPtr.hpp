/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__templateAutoPtr_HPP__
#define INCLUDED__templateAutoPtr_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

/*!
Automatic Scope-Lifetime pointer wrapper templates
---------------------------------------------------
 AAutoPtr           - for single object types
 AAutoArrayPtr      - for an array of object
 AAutoBasicArrayPtr - for an array of basic types

 Example AutoPtr:
   AAutoPtr<MyObject> pOb(new MyObject);    //Single object deleted when leaves scope

 Equals to:
   MyObject* pOb = new MyObject;
   delete pOb;


 Example AutoPtr:
   AAutoArrayPtr<MyObject> pOb[3];           //Array of objects deleted when leaves scope
   pOb[0] = new MyObject;
   pOb[1] = new MyObject;
   pOb[2] = new MyObject;

 Equals to:
   MyObject* pOb = new MyObject;
   pOb[0] = new MyObject;
   pOb[1] = new MyObject;
   pOb[2] = new MyObject;
   delete[] pOb;

 Example AutoPtr:
   AAutoBasicArrayPtr<char> pcBuf(new char[128]);   //Array of basic types deleted when leaves scope
   strcpy(pcBuf, "HelloWorld");

 Equals to:
   char* pcBuf = new char[128];
   strcpy(pcBuf, "HelloWorld");
   delete[] pcBuf;


 Using AutoArrayPtr<> as a dynamic array of AAutoPtr<>
 This will automatically deallocate the array and all of its members when it leaves scope
   int COUNT = 6;
   AAutoArrayPtr< AAutoPtr<MyObject> > objectArray;
   objectArray = new AutoStatPtr[COUNT];
   for(int i=0; i < COUNT; ++i)
     objectArray[i] = new MyObject;
   ...
   objectArray[i]->callSomething();

 Equals to:
   int COUNT = 6;
   MyObject *objectArray = new MyObject[COUNT];
   for(int i=0; i < COUNT; ++i)
     objectArray[i] = new MyObject;
   ...
   objectArray[i]->callSomething();
   ...
   for(int i=0; i < COUNT; ++i)
     delete objectArray[i];
   delete []objectArray;
*/
template <class T>
class AAutoPtr : public ADebugDumpable
{
public:
  /*!
  Default ctor points to NULL and owns nothing
  */
  AAutoPtr() : m_Pointer(NULL), m_Ownership(false) {}

  /*!
  Pointer to an object (NOT to be used with arrays, see below templates)
  */
	explicit AAutoPtr(T* pointer, bool ownership) : m_Pointer(pointer), m_Ownership(ownership)
  {
  }

  /*!
  dtor
  */
  ~AAutoPtr()
  { 
    if (m_Ownership && m_Pointer)
    {
      delete m_Pointer;
    }
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
  Reset the pointer and delete the old pointer if owned
  */
  void reset(T* pointer = NULL, bool ownership = true)
  { 
    if (m_Ownership)
    {
      delete m_Pointer;
    }
    m_Pointer = pointer;
    m_Ownership = ownership;
  }
	
  /*!
  Pointer operator
  */
  T* operator ->() { return m_Pointer; }

  /*!
  Const pointer operator
  */
  const T* operator ->() const { return m_Pointer; }

  /*!
  Cast operator
  */
  operator T* () { return m_Pointer; }
  
  /*!
  Const cast operator
  */
  operator const T* () const { return m_Pointer; }

  /*!
  Use pointer
  */
  T* use() { return m_Pointer; }

  /*!
  Get const pointer
  */
  const T* const get() const { return m_Pointer; }

  /*!
  Derefencing operator
  */
  T& operator *() { return *m_Pointer; }

  /*!
  Checks if contained pointer is not obviously invalid
  */
  bool isPointerValid() const { return ADebugDumpable::isPointerValid(m_Pointer); }

  /*!
  Checks if contained pointer is NULL
  */
  bool isNull() const { return (NULL == m_Pointer); }

  /*!
  Checks if contained pointer is not NULL
  */
  bool isNotNull() const { return (NULL != m_Pointer); }

  /*!
  Change of ownership
  */
  void setOwnership(bool ownership = true) { m_Ownership = ownership;}

  /*!
  AEmittable
  */
  void emit(AOutputBuffer& target) const
  {
    target.append('{');
    target.append(AString::fromPointer(m_Pointer));
    if (m_Ownership)
      target.append(";owned",6);
    target.append('}');
  }

private:
  //! No copy ctor
  AAutoPtr(AAutoPtr&) {}

  //! No operator=
  AAutoPtr& operator=(const AAutoPtr&) { return *this; }

	T*   m_Pointer;
	bool m_Ownership;

public:
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const
  {
    ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << ") ptr=" << AString::fromPointer(m_Pointer) << " owned=" << AString::fromBool(m_Ownership) << std::endl;
  }
};


/*!
AAutoArrayPtr class ABASE_API is meant for pointers to arrays of classes
*/
template <class T>
class AAutoArrayPtr : public ADebugDumpable
{
public:
  /*!
  Default ctor points to NULL and owns nothing
  */
  AAutoArrayPtr() : m_Pointer(NULL), m_Ownership(false) {}

  /*!
  ctor
  */
  explicit AAutoArrayPtr(T* pointer, bool ownership) : m_Pointer(pointer), m_Ownership(ownership) {}
	
  /*!
  dtor
  */
  ~AAutoArrayPtr()
  { 
    if (m_Ownership) delete []m_Pointer;
  }

  /*!
  Assign to a pointer and take ownership
  */
  AAutoArrayPtr& operator=(T* pointer)
  {
    reset(pointer);
    return *this;
  }

  /*!
  Reset the pointer and delete the old pointer if owned
  */
  void reset(T* pointer = NULL, bool ownership = true)
  { 
    if (m_Ownership)
      delete[] m_Pointer;
    m_Pointer = pointer;
    m_Ownership = ownership;
  }
	
  /*!
  Cast operator
  */
  operator T* () { return m_Pointer; }
  
  /*!
  Const cast operator
  */
  operator const T* () const { return m_Pointer; }

  /*!
  Use pointer
  */
  T* use() { return m_Pointer; }

  /*!
  Get const pointer
  */
  const T* const get() const { return m_Pointer; }

  /*!
  Derefencing operator
  */
  T& operator *() { return *m_Pointer; }

  /*!
  Array access operators
  */
  T &operator [](int iOffset) { return m_Pointer[iOffset]; }

  /*!
  Checks if contained pointer is not obviously invalid
  */
  bool isPointerValid() const { return ADebugDumpable::isPointerValid(m_Pointer); }

  /*!
  Checks if contained pointer is NULL
  */
  bool isNull() const { return (NULL == m_Pointer); }

  /*!
  Checks if contained pointer is not NULL
  */
  bool isNotNull() const { return (NULL != m_Pointer); }

  /*!
  Change of ownership
  */
  void setOwnership(bool ownership = true) { m_Ownership = ownership;}

  /*!
  AEmittable
  */
  void emit(AOutputBuffer& target) const
  {
    target.append('{');
    target.append(AString::fromPointer(m_Pointer));
    if (m_Ownership)
      target.append(";owned",6);
    target.append('}');
  }

private:
  //! No copy ctor
  AAutoArrayPtr(AAutoArrayPtr&) {}

  //! No operator=
  AAutoArrayPtr& operator=(const AAutoArrayPtr&) { return *this; }

  T*   m_Pointer;
	bool m_Ownership;

public:
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const
  {
    ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << ") ptr=" << AString::fromPointer(m_Pointer) << " owned=" << AString::fromBool(m_Ownership) << std::endl;
  }
};


/*!
AAutoBasicArrayPtr class is meant for pointers to arrays of basic types (ie. char *)
*/
template <class T>
class AAutoBasicArrayPtr : public ADebugDumpable
{
public:
  /*!
  Default ctor points to NULL and owns nothing
  */
  AAutoBasicArrayPtr() : m_Pointer(NULL), m_Ownership(false) {}

  /*!
  ctor
  */
  explicit AAutoBasicArrayPtr(T* pointer = NULL, bool ownership = true) : m_Pointer(pointer), m_Ownership(ownership) {}
  
  /*!
  dtor
  */
  ~AAutoBasicArrayPtr()
  { 
    if (m_Ownership)
      delete[] m_Pointer;
  }
 
  /*!
  Equals operator with ownership
  */
  AAutoBasicArrayPtr& operator=(T* pointer)
  {
    reset(pointer);
    return *this;
  }

  /*!
  Reset the pointer and delete the old pointer if owned
  */
  void reset(T* pointer = NULL, bool ownership = true)
  {
    if (m_Ownership) 
      delete[] m_Pointer;
    
    m_Pointer = pointer; 
    m_Ownership = ownership;
  }

  /*!
  Cast operator
  */
  operator       T* ()       { return m_Pointer; }
  
  /*!
  Const cast operator
  */
  operator const T* () const { return m_Pointer; }

  /*!
  Use pointer
  */
  T* use()                    { return m_Pointer; }

  /*!
  Get const pointer
  */
  const T* const get() const  { return m_Pointer; }

  /*!
  Derefencing operator
  */
  T& operator *()             { return *m_Pointer; }

  /*!
  Array access operators
  */
  T& operator [](int iOffset) { return m_Pointer[iOffset]; }

  /*!
  Checks if contained pointer is not obviously invalid
  */
  bool isPointerValid() const { return ADebugDumpable::isPointerValid(m_Pointer); }

  /*!
  Checks if contained pointer is NULL
  */
  bool isNull() const { return (NULL == m_Pointer); }

  /*!
  Checks if contained pointer is not NULL
  */
  bool isNotNull() const { return (NULL != m_Pointer); }

  /*!
  Change of ownership
  */
  void setOwnership(bool ownership = true) { m_Ownership = ownership;}

  /*!
  AEmittable
  */
  void emit(AOutputBuffer& target) const
  {
    target.append('{');
    target.append(AString::fromPointer(m_Pointer));
    if (m_Ownership)
      target.append(";owned",6);
    target.append('}');
  }

private:
  //! No copy ctor
  AAutoBasicArrayPtr(AAutoBasicArrayPtr&) {}

  //! No operator=
  AAutoBasicArrayPtr& operator=(const AAutoBasicArrayPtr&) { return *this; }

  T*   m_Pointer;
  bool m_Ownership;

public:
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const
  {
    ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << ") ptr=" << AString::fromPointer(m_Pointer) << " owned=" << AString::fromBool(m_Ownership) << std::endl;
  }
};

#endif
