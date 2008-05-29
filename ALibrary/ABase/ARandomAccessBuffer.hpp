/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ARandomAccessBuffer_HPP_
#define INCLUDED__ARandomAccessBuffer_HPP_

#include "apiABase.hpp"

class ABASE_API AOutputBuffer;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  //! ARandomAccessBuffer
  virtual size_t access(AOutputBuffer& target, size_t index = 0, size_t bytes = AConstant::npos) const;
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API ARandomAccessBuffer
{
public:
  virtual ~ARandomAccessBuffer();

  /*!
  Peek the contents of the file without changing read position

  @param target of the peek
  @param index start position of the read
  @param bytes to read  (AConstant::npos means all bytes to EOF)
  */
  virtual size_t access(AOutputBuffer& target, size_t index = 0, size_t bytes = AConstant::npos) const = 0;
};

#endif //INCLUDED__ARandomAccessBuffer_HPP_
