#include "pchABase.hpp"
#include "ABitArray.hpp"
#include "ATextConverter.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"

void ABitArray::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  AString str(32, 16);
  for (size_t x=0; x<m_size; ++x)
  {
    if ((x % 32) == 0)
      ADebugDumpable::indent(os, indent);
    if (x % 8 == 0)
      str.append(" ");
    str.append(isSet(m_size-x-1) ? "1" : "0");
    if ((x % 32) == 31)
    {
      os << str << " :" << x << std::endl;
      str.clear();
    }
  }
  ADebugDumpable::indent(os, indent+1) << "m_OutputMode=" << m_OutputMode << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABitArray::ABitArray() :
  mp_bits(NULL),
  m_physical(0),
  m_size(0),
  m_OutputMode(ABitArray::Hexadecimal)
{
}

ABitArray::ABitArray(size_t size) :
  mp_bits(NULL),
  m_physical(0),
  m_size(0),
  m_OutputMode(ABitArray::Hexadecimal)
{
  setSize(size);
}

ABitArray::ABitArray(
  const AString& strBitset, 
  ABitArray::Mode mode // = ABitArray::Hexadecimal
) :
  mp_bits(NULL),
  m_physical(0),
  m_size(0),
  m_OutputMode(mode)
{
  set(strBitset, mode);
}

ABitArray::ABitArray(const ABitArray& that) :
  mp_bits(NULL),
  m_physical(0),
  m_size(0),
  m_OutputMode(that.m_OutputMode)
{
  setSize(that.getSize());
  if (that.mp_bits)
  {
    AASSERT(this, mp_bits);
    memcpy(mp_bits, that.mp_bits, m_physical); 
  }
}
  
ABitArray::~ABitArray()
{
  delete []mp_bits;
}

void ABitArray::setSize(size_t newsize)
{   
  size_t allocsize = (newsize >> 3) + 1;
  u1 *pt, *pnew;
  
  if (allocsize > m_physical)
  {
    AASSERT(this, allocsize < DEBUG_MAXSIZE_ABitArray);
    pnew = new u1[allocsize];
    if (mp_bits)
      memcpy(pnew, mp_bits, m_physical);

    pt = mp_bits;
    mp_bits = pnew;
    delete []pt;

    for (; m_physical<allocsize; ++m_physical)
      mp_bits[m_physical]=u1(0x0);
  }

  m_size = newsize;

  //TODO: setSize shrink case should reallocate maybe to save space
}

bool ABitArray::isSet(size_t index) const
{  
  size_t cell = index >> 3;
  if (cell >= m_physical)
    return false;               //a_Bit not set yet probably

  size_t offset = index - ((index >> 3) << 3);

  return (_getBit(cell, offset) ? true : false);
}

bool ABitArray::isClear(size_t index) const
{  
  size_t cell = index >> 3;
  if (cell >= m_physical)
    return true;               //a_Bit not set yet probably

  size_t offset = index - ((index >> 3) << 3);

  return (_getBit(cell, offset) ? false : true);
}

void ABitArray::setBit(size_t index, bool value /* =true */)
{
  if (index >= m_size)
    setSize(index);
  
  size_t cell = index >> 3;
  size_t offset = index - ((index >> 3) << 3);

  _setBit(cell, offset, value);
}

void ABitArray::all(u1 value /* =0 */)
{
  if (mp_bits && m_physical)
  {
    u1 mask = 0x00;
    if (value)
      mask = 0xff;
      
    memset(mp_bits, mask, m_physical);
  }
}

void ABitArray::clear(size_t newsize /*= 0*/, u1 value /*= 0*/)
{
  setSize(newsize);
  all(value);
}

void ABitArray::set(const AString& strBitset, ABitArray::Mode mode /* = ABitArray::Hexadecimal */)
{
  size_t size = strBitset.getSize();
  
  switch (mode)
  {
    case ABitArray::Binary:
    {
      clear(size, 0);
      for (size_t x=0; x < size; ++x)
        if (strBitset.at(size - x - 1) != '0')
          setBit(x, 1);
    }
    break;

    case ABitArray::Hexadecimal:
    {
      //a_Each char is 4 bits 0-F, 2 chars is 1 byte (8 bits)  e.g. "3C" = 0x3c
      clear(size * 4, 0x0);                
      size_t x = 0, offset;
      for (size_t t=0; t<size; t+=2)
      {
        offset = size - t - 1;

        //a_XOR is used to invert the bits
        if (offset > 0)
          _setByte(x, ATextConverter::convertHEXtoBYTE(strBitset.at(offset-1), strBitset.at(offset)));
        else
          _setByte(x, ATextConverter::convertHEXtoBYTE('0', strBitset.at(offset)));

        ++x;
      }
    }
    break;

    default:
      ATHROW(this, AException::NotImplemented);
  }
}

void ABitArray::get(AOutputBuffer& str, ABitArray::Mode mode /* = ABitArray::Hexadecimal */) const
{
  switch (mode)
  {
    case ABitArray::Binary:
    {
      for (size_t x=0; x<m_size; ++x)
        if (isSet(m_size-x-1))
          str.append('1');
        else
          str.append('0');
    }
    break;

    case ABitArray::Hexadecimal:
    {
      u1 value;
      size_t bytes = (m_size+7) >> 3;
      for (size_t x=0; x<bytes; ++x)
      {
        value = u1(_getByte(bytes-x-1));
        ATextConverter::convertBYTEtoHEX(value, str);
      }
    }
    break;

    default:
      ATHROW(this, AException::NotImplemented);
  }
}

void ABitArray::emit(AOutputBuffer& target) const
{
  ARope rope;
  get(rope, m_OutputMode);
  target.append(rope);
}

AXmlElement& ABitArray::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  ARope rope;
  get(rope, m_OutputMode);
  thisRoot.addElement(ASW("data",4)).addData(rope, AXmlElement::ENC_CDATADIRECT);
  return thisRoot;
}

void ABitArray::setOutputMode(
  Mode mode // = ABitArray::Hexadecimal
)
{
  m_OutputMode = mode;
}

