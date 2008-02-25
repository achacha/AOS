#ifndef INCLUDED__AOutputBuffer_HPP__
#define INCLUDED__AOutputBuffer_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AEmittable.hpp"

class AString;
class AFile;

/*!
Class to define any class that can be used to output data

-----------------------OVERLOAD---------------------------
public:
  virtual size_t flush(AFile&);
protected:
  virtual void _append(const char *pccSource, u4 len) = 0;
-----------------------OVERLOAD---------------------------
*/
class ABASE_API AOutputBuffer : public ADebugDumpable
{
public:
  /*!
  Output methods
  */
  void append(const char *);
  void append(const char *, size_t len);
  void append(const void *, size_t len);
  void append(const AEmittable&);
  void append(const AString&, size_t len);     //a_Append first len bytes
  void append(char data);

  /*!
  Network order (Big endian, MSB) append of internal types
  */
  void appendU1(u1);
  void appendU2(u2);
  void appendU4(u4);
  void appendU8(u8);
  void appendInt(int);

  /*!
  Little endian (LSB) append of internal types
  Used in some rare cases, network order is the prefered way
  Did you wonder why U1_LSB was missing?!
  */
  void appendU2_LSB(u2);
  void appendU4_LSB(u4);
  void appendU8_LSB(u8);
  void appendInt_LSB(int);

  /*!
  Append string and EOL
  */
  void appendLine(const AEmittable&);  //a_Appends the string and AConstant::ASTRING_EOL

  /*!
  Ability to flush the entire content into AFile&
  Whatever was written will be removed from the buffer
  @return bytes written (0 or more)
          AConstant::unavail if the call did not write anything and should be retried (non-blocking IO)
          AConstant::npos if target EOF or unable to write
  */
  virtual size_t flush(AFile&) = 0;

  /*!
  Gets the size of the data in the buffer
  @return bytes in a buffer or AConstant::npos if not applicable
  */
  virtual size_t getSize() const = 0;

protected:
  virtual void _append(const char *pccSource, size_t len) = 0;
};

#endif //INCLUDED_AOutputBuffer_HPP_
