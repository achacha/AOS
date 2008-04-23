#ifndef INCLUDED_ABitArray_HPP_
#define INCLUDED_ABitArray_HPP_

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"

/*!
USAGE: ABitArray ar(8);
   ar.set(0,1);
   ar.set(1,1);
   ar.set(3,1);
   ar.get(str, ABitArray::Hexadecimal);
   str => "000B"
   ar.get(str, ABitArray::Binary);
   str => "00000000 00000000 00000000 00001011"  (no spaces)

   ar.set("cafe");
   ar.set(0,1);
   ar.get(str, ABitArray::Hexadecimal);
   ar => "caff"

NOTE: If you start off as Binary Mode, then get as hex and set it back as hex (i.e. save/restore via hex)
        you will increase the size to the next highest boundary of 8
*/
class ABASE_API ABitArray : public ADebugDumpable, public AXmlEmittable
{
public:
  enum Mode
  {
    Hexadecimal,
    Binary
  };

public:
  //! ctor
  ABitArray();
  
  //! ctor with initial size allocation in bits
  ABitArray(size_t size);

  /*!
  ctor that parses a string bitset
  hex is "7b2cca130..."
  binary is "10011100011..."
  */
  ABitArray(const AString& strBitset, ABitArray::Mode mode = ABitArray::Hexadecimal);
  
  //! copy ctor
  ABitArray(const ABitArray&);
  
  //! virtual dtor
  virtual ~ABitArray();

  /*!
  Size is in bits always
  */
  void setSize(size_t newsize);
  size_t getSize() const;

  /*!
  Bitwise access
  */
  bool isSet(size_t index) const;
  bool isClear(size_t index) const;
  void setBit(size_t index, bool value = true);
  void clearBit(size_t index);

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
  
  //! AEmittable
  virtual void emit(AOutputBuffer& target) const;
  
  //! AXmlEmittable
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;
  
  /*!
  Set output method to be used wne emitting
  You can parse a binary string, then output it as hex, et. al
  */
  void setOutputMode(Mode Mode = ABitArray::Hexadecimal);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  void _setBit(size_t cell, size_t offset, bool value);
  void _setByte(size_t cell, u1 value);
  bool _getBit(size_t cell, size_t offset) const;
  u4   _getByte(size_t cell) const;

  size_t m_physical;      //a_Amount physically allocated array (this is the most important attribute, the pointer to data is secondary), 0 means empty
  u1 *mp_bits;            //a_Data array 8 bits per cell
  size_t m_size;          //a_Size 2^N max (N is the number of bits of the processor compiled on ie. sizeof(size_t))
  Mode m_OutputMode;      //a_How output is to be displayed
};

#endif //INCLUDED_ABitArray_HPP_
