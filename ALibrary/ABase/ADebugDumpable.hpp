/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ADebugDumpable_HPP__
#define INCLUDED__ADebugDumpable_HPP__

#include "apiABase.hpp"
#include "AEmittable.hpp"

/*!
Inheriting from this class gives you a common interface for debugging dump
Default behavior will just dump the typeid name and memory location of the object

Implementation sample:
Add this to your class after you inherit from ADebugDumpable if you have members variable to display:


-----------------------START: HPP cut/paste-----------------------------------
class MyClass : public ADebugDumpable
{
  ...

  // From AEmittable
  virtual void emit(AOutputBuffer&) const;
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

  ...
};
-----------------------STOP: HPP cut/paste-----------------------------------

-----------------------START: CPP cut/paste-----------------------------------
void MyClass::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  //ADebugDumpable::indent(os, indent+1) << "myname=" << m_name << std::endl;      //TODO:
  //ADebugDumpable::indent(os, indent+1) << "myvalue=" << m_value << std::endl;    //TODO:
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void MyClass::emit(AOutputBuffer& target) const
{
}
-----------------------STOP: CPP cut/paste-----------------------------------

*/
class ABASE_API ADebugDumpable : public AEmittable
{
public:
  /*!
  Debug dump to output stream
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  
  /*!
  Helper to dump to AOutputBuffer by calling debugDump and redirecting
  */
  void debugDumpToAOutputBuffer(AOutputBuffer&, int indent = 0x0) const;   //a_Helper method that wraps os and calls the virtual one

public:
  /*!
  AEmittable
  */
  void emit(AOutputBuffer&) const;
  
  /*!
  Indents
  */
  static std::ostream& indent(std::ostream&, size_t indent);

  /*!
  Dumps memory of given length to an ostream
  Shows both hex bytes and ASCII
  This function is only for debugging purposes
  indent is # of indent blocks to offset
  */
  static void dumpMemory_HexAscii(std::ostream&, const void *pvObject, size_t objectSize, size_t indent = 0, size_t bytesPerRow = 16);

  /*!
  Dumps memory of given length to an ostream
  Shows both hex bytes and ASCII
  This function is only for debugging purposes
  indent is # of indent blocks to offset
  */
  static void dumpMemory_Ascii(std::ostream&, const void *pvObject, size_t objectSize, size_t indent = 0, size_t bytesPerRow = 32);

  /*!
  Dumps memory of given length to an ostream
  Shows hex bytes
  This function is only for debugging purposes
  indent is # of indent blocks to offset
  */
  static void dumpMemory_Hex(std::ostream&, const void *pvObject, size_t objectSize, size_t indent = 0, size_t bytesPerRow = 16);

  /*!
  Platform specific debug output
  */
  static void trace(const char *pcc);

  /*!
  Checks if a pointer is valid (platform specific of course and not all platforms support this)
  */
  static bool isPointerValid(void *);
};

#endif //INCLUDED__ADebugDumpable_HPP__
