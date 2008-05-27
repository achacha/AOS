/*
Written by Alex Chachanashvili

Id: $Id$
*/
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
  @return number of bytes actually written
          AConstant::npos - error during append (buffer closed, etc)
          AConstant::unavail - buffer full at the moment, retry (only applicable to non-blocking sockets at the moment)
  */
  size_t append(const char *);
  size_t append(const char *, size_t len);
  size_t append(const void *, size_t len);
  size_t append(const AString&, size_t len);     //a_Append first len bytes
  size_t append(char data);

  /*!
  Append emittable
  */
  void append(const AEmittable&);

  /*!
  Network order (Big endian, MSB) append of internal types
  Appends actual bytes to the buffer
  Not to be confused with converting a number to string and appending, that is done by AString::fromXXXX() methods
  */
  size_t appendU1(u1);
  size_t appendU2(u2);
  size_t appendU4(u4);
  size_t appendU8(u8);

  /*!
  Little endian (LSB) append of internal types
  Used in some rare cases, network order is the prefered way
  Did you wonder why U1_LSB was missing?!
  */
  size_t appendU2_LSB(u2);
  size_t appendU4_LSB(u4);
  size_t appendU8_LSB(u8);

  /*!
  Append string and EOL
  */
  void appendLine(const AEmittable&);  //a_Appends the string and AConstant::ASTRING_EOL

  /*!
  Gets the size of the data in the buffer
  @return bytes in a buffer or AConstant::npos if not applicable
  */
  virtual size_t getSize() const = 0;

protected:
  /*!
  Append data
  @return number of bytes written
          AConstant::npos - error during append (buffer closed, etc)
          AConstant::unavail - buffer full at the moment, retry (only applicable to non-blocking sockets at the moment)
  */
  virtual size_t _append(const char *pccSource, size_t len) = 0;
};

#endif //INCLUDED_AOutputBuffer_HPP_
