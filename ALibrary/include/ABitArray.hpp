#ifndef INCLUDED_ABitArray_HPP_
#define INCLUDED_ABitArray_HPP_

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"

//a_USAGE: ABitArray ar(8);
//         ar.set(0,1);
//         ar.set(1,1);
//         ar.set(3,1);
//         ar.get(str, ABitArray::Hexadecimal);
//         str => "000B"
//         ar.get(str, ABitArray::Binary);
//         str => "00000000 00000000 00000000 00001011"  (no spaces)
//
//         ar.set("cafe");
//         ar.set(0,1);
//         ar.get(str, ABitArray::Hexadecimal);
//         ar => "caff"
//
//a_NOTE: If you start off as Binary Mode, then get as hex and set it back as hex (i.e. save/restore via hex)
//        you will increase the size to the next highest boundary of 8
class ABASE_API ABitArray : public ADebugDumpable, public AXmlEmittable
{
public:
  enum Mode
  {
    Hexadecimal,
    Binary
  };

public:
  ABitArray();
  ABitArray(size_t size);
  ABitArray(const AString& strBitset, ABitArray::Mode mode = ABitArray::Hexadecimal);
  ABitArray(const ABitArray&);
  virtual ~ABitArray();

  /*!
  Size is in bits always
  */
  void setSize(size_t newsize);
  size_t getSize() const { return m_size; }

  /*!
  Bitwise access
  */
  bool isSet(size_t index) const;
  bool isClear(size_t index) const;
  void setBit(size_t index, bool value = true);
  void clearBit(size_t index) { setBit(index, 0); }

  /*!
  Sets the entire array and sets value to 'value'
  */
  void all(u1 value = 0);

  /*!
  Sets sets the new size and sets array to 'value'
  */
  void clear(size_t newsize = 0, u1 value = 0);

  /*!
  Block get/set
    hex stream  e.g. "8f3a7e33", binary stream  e.g. "100100111001"
  */
  void get(AOutputBuffer&, ABitArray::Mode mode = ABitArray::Hexadecimal) const;
  void set(const AString& strBitset, ABitArray::Mode mode = ABitArray::Hexadecimal);
  
  /*!
  AEmittable and AXmlEmittable and output mode
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;
  void setOutputMode(Mode Mode = ABitArray::Hexadecimal);

private:
  void _setBit(size_t cell, size_t offset, bool value);
  void _setByte(size_t cell, u1 value);
  bool _getBit(size_t cell, size_t offset) const;
  u4   _getByte(size_t cell) const;

  size_t m_physical;      //a_Amount physically allocated array (this is the most important attribute, the pointer to data is secondary), 0 means empty
  u1 *mp_bits;            //a_Data array 8 bits per cell
  size_t m_size;          //a_Size 2^N max (N is the number of bits of the processor compiled on ie. sizeof(size_t))
  Mode m_OutputMode;      //a_How output is to be displayed

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

ABASE_API inline void ABitArray::_setBit(size_t cell, size_t offset, bool value)
{
  if (value) mp_bits[cell] |= u4(0x1) << offset;
  else mp_bits[cell] &= ~(u4(0x1) << offset);
}

ABASE_API inline void ABitArray::_setByte(size_t cell, u1 value)
{
  mp_bits[cell] = value;
}

ABASE_API inline bool ABitArray::_getBit(size_t cell, size_t offset) const
{
  return (((mp_bits[cell] >> offset) & 0x1) != 0);
}

ABASE_API inline u4 ABitArray::_getByte(size_t cell) const
{
  return mp_bits[cell];
}

#endif //INCLUDED_ABitArray_HPP_
