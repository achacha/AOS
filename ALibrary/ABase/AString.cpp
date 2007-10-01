#include "pchABase.hpp"
#include "AString.hpp"
#include "ANumber.hpp"
#include "templateSingletonPattern.hpp"
#include "AException.hpp"
#include "AFile.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"

#include <ctype.h>

#ifdef __DEBUG_DUMP__
void AString::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AString @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Length=" << m_Length << ":" << m_InternalBufferSize << ":" << m_BufferIncrement << "  getHash()=" << getHash() << std::endl;
  bool ascii = true;
  for(size_t i=0; i<m_Length; ++i)
  {
    if (mp_Buffer[i] < 20 || mp_Buffer[i] > 127)
    {
      ascii=false;
      break;
    }
  }

  if (m_Length)
  {
    if (ascii)
      ADebugDumpable::indent(os, indent+1) << "mp_Buffer=" << mp_Buffer << std::endl;
    else
    {
      ADebugDumpable::indent(os, indent+1) << "mp_Buffer=" << std::hex << (void*)mp_Buffer << std::dec << " {" << std::endl;
      ADebugDumpable::dumpMemory_HexAscii(os, mp_Buffer, m_Length, indent+2); 
      ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
    }
  }
  ADebugDumpable::indent(os,indent+1) << "mbool_Wrapped=" << (mbool_Wrapped ? "true" : "false") << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

//a_The one and only NULL pos static!
const u2 AString::smi_DefaultBufferIncrement = 256;

AString::AString
(
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
}

AString::AString
(
  const char* pccSource, 
  size_t length
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
  _assign(pccSource, length);
}

AString::AString
(
  const wchar_t* pwccSource,
  size_t length
) :
  mp_Buffer(NULL),
  m_Length(0x0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0x0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
  assignDoubleByte(pwccSource, length);
}

AString::AString
(
  const char cSource
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
  _assign(&cSource, 1);
}

AString::AString
(
  size_t length,
  u2 increment
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(increment)
{
  AASSERT(this, increment > 0);

  //a_Adjust the size
  _resize(length);   //a_Allocate space
  m_Length = 0;      //a_No content
}

AString::AString
(
  const AString& source
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(source.m_BufferIncrement)
{
  //if (source.mbool_Wrapped)
  //{
  //  //a_Retain wrapping
  //  mp_Buffer = source.mp_Buffer;
  //  m_Length = source.m_Length;
  //  mbool_Wrapped = source.mbool_Wrapped;
  //  m_InternalBufferSize = source.m_InternalBufferSize;
  //}
  //else
  //{
    //a_Copy content (if any)
    if (source.m_Length > 0)
      _assign(source.mp_Buffer, source.m_Length);
//  }
}

AString::AString
(
  const AEmittable& source
) :
  mp_Buffer(NULL),
  m_Length(0),
  mbool_Wrapped(false),
  m_InternalBufferSize(0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
  source.emit(*this);
}

AString::AString(
  char *pcSource, 
  size_t length, 
  bool
) :
  mp_Buffer(pcSource),
  m_Length(length),
  mbool_Wrapped(true),
  m_InternalBufferSize(0),
  m_BufferIncrement(AString::smi_DefaultBufferIncrement)
{
  //a_Wrapped object, get length if AConstant::npos
  if (AConstant::npos == length)
    m_Length = strlen(mp_Buffer);
}

const AString AString::wrap(
  const char *pccSource, 
  size_t length // = AConstant::npos
)
{
  if (!pccSource)
    return AConstant::ASTRING_EMPTY;
 
  //a_const_cast<> here is a temporary removal of constness
  //a_Used to construct the class, return is a const AString so pccSource is never changed
  return AString(const_cast<char *>(pccSource), length, true);
}

AString::~AString()
{
  if (!mbool_Wrapped)
    _deallocate(&mp_Buffer);
}

char *AString::_allocate(size_t bytes)
{
  AASSERT_EX(this, !mbool_Wrapped, ASWNL("AString::_allocate: Cannot resize a constant string wrapper"));
  AASSERT(this, bytes > m_Length);   //a_Should only allocate if we really need more
  AASSERT(this, bytes < DEBUG_MAXSIZE_AString);  //a_Debug only limit

  return new char[bytes];
}
void AString::_deallocate(char **ppcBuffer)
{
  //a_Release non-NULL 
  AASSERT(this, !mbool_Wrapped);
  delete[](*ppcBuffer);
}

char AString::mc_Null = '\x0';

//a_Static methods
AString AString::fromInt(int iValue)
{
  char pcBuffer[18];

#if (_MSC_VER >= 1400)
  errno_t errornum = _itoa_s(iValue, pcBuffer, 18, 0xA);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(itoa(iValue, pcBuffer, 0xA));
#endif
}

AString AString::fromBool(bool value)
{
  return (value ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE);
}

AString AString::fromS1(s1 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ltoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ltoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromU1(u1 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ultoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ultoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromS2(s2 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ltoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ltoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromU2(u2 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ultoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ultoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromS4(s4 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ltoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ltoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromU4(u4 value, int iBase)
{
  char pcBuffer[34];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ultoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(ultoa(value, pcBuffer, iBase));
#endif
}

AString AString::fromS8(s8 value, int iBase)
{
  char pcBuffer[65];

#if (_MSC_VER >= 1400)
  errno_t errornum = _i64toa_s(value, pcBuffer, 65, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(_i64toa(value, pcBuffer, iBase));
#endif;
}

AString AString::fromU8(u8 value, int iBase)
{
  char pcBuffer[65];

#if (_MSC_VER >= 1400)
  errno_t errornum = _ui64toa_s(value, pcBuffer, 65, iBase);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
  return AString(pcBuffer);
#else
  return AString(_ui64toa(value, pcBuffer, iBase));
#endif;
}

AString AString::fromDouble(
  double value, 
  int precision,       // = 3
  bool forcePrecision  // = false
)
{
  ANumber n(value);
  if (precision >= 0)
  {
    if (forcePrecision)
      n.forcePrecision(precision);
    else
      n.setPrecision(precision);
  }

  return n.toAString();
}

AString AString::fromSize_t(size_t value, int iBase)
{
  switch(sizeof(value))
  {
    case 8: return fromU8(value, iBase);
    case 4: return fromU4(value, iBase);
    default: ATHROW(NULL, AException::ProgrammingError);
  }
}

AString AString::fromPointer(const void* pValue)
{
  char pcBuffer[70];
  pcBuffer[0x0] = '0';
  pcBuffer[0x1] = 'x';

#if (_MSC_VER >= 1400)
  errno_t errornum = _ui64toa_s((u8)pValue, pcBuffer+2, 68, 0x10);
  if (errornum)
    ATHROW_ERRNO(NULL, AException::APIFailure, errornum);
#else
  _ui64toa((u8)pValue, pcBuffer+2, 0x10);
#endif
  return AString(pcBuffer);
}

//a_Local methods
void AString::parseInt(int value)
{
  char pcBuffer[18];
#if (_MSC_VER >= 1400)
  errno_t errornum = _itoa_s(value, pcBuffer, 18, 0xA);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
  _assign(pcBuffer, AConstant::npos);
#else
  _assign(ltoa(value, pcBuffer, 0xA), AConstant::npos);
#endif
}

void AString::parseS4(s4 value, int iBase)
{
  char pcBuffer[34];
#if (_MSC_VER >= 1400)
  errno_t errornum = _ltoa_s(value, pcBuffer, 34, iBase);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
  _assign(pcBuffer, AConstant::npos);
#else
  _assign(ltoa(value, pcBuffer, iBase), AConstant::npos);
#endif
}

void AString::parseU4(u4 value, int iBase)
{
  char pcBuffer[34];
#if (_MSC_VER >= 1400)
  errno_t errornum = _ultoa_s(value, pcBuffer, iBase);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
  _assign(pcBuffer, AConstant::npos);
#else
  _assign(ultoa(value, pcBuffer, iBase), AConstant::npos);
#endif
}

void AString::parsePointer(const void* pValue)
{
  char pcBuffer[70];
  pcBuffer[0] = '0';
  pcBuffer[1] = 'x';

#if (_MSC_VER >= 1400)
  _ui64toa_s((u8)pValue, pcBuffer + 2, 70, 0x10);
#else
  _ui64toa((u8)pValue, pcBuffer + 2, 0x10);
#endif

  _assign(pcBuffer, AConstant::npos);
}

AString& AString::operator= (const AString& source)
{ 
  if (this != &source)
    if (source.isEmpty())
      clear();
    else
      _assign(source.mp_Buffer, source.m_Length); 

  return *this;
}

void AString::_assign(const char* pccSource, size_t length)
{
  if (!pccSource)
  {
    //a_Assigned a null, release memory also
    if (m_Length) clear(true);
    return;
  }
  
  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);
 
  //a_Check for empty string
  if (length == 0)
  {
    m_Length = 0;
    return;
  }
  
  //a_Resize the buffer
  _resize(length);

  //a_Copy contents of the source
  if (length > 0)
  {
    memcpy(mp_Buffer, pccSource, length);
    mp_Buffer[length] = '\x0';
    m_Length = length;
  }
  else
    m_Length = 0;
}

int AString::compare(const char* pccSource, size_t length /* = AConstant::npos */) const
{ 
  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);
  
  return _compare(pccSource, length);
}

int AString::_compare(const char* pccSource, size_t length, bool boolNoCase) const
{
  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);
  
  //a_Empty string
  if (!m_Length)
  {
    //a_Check if one is empty
    if (!length)
      return 0;
    else
      return -1;
  }
  else
  {
    if (!length)
    {
      //a_Source length is zero, we are bigger
      return 1;   
    }
    else
    {
      AASSERT(this, mp_Buffer);

      if (!pccSource)
        ATHROW(this, AException::SourceBufferIsNull);
    
      //a_Use the smaller one
      size_t compareLength = (m_Length > length ? length : m_Length);

      //a_Compare
      int ret;
      if (boolNoCase)
        ret = _strnicmp(mp_Buffer, pccSource, compareLength);
      else
        ret = strncmp(mp_Buffer, pccSource, compareLength);
      
      //a_Different size but equal (one is longer)
      if (
           (m_Length != length) && 
           (!ret)
         )
      {
        return (m_Length > length ? INT_MAX : INT_MIN);
      }
      else
        return ret;
    }
  }
}

size_t AString::diff(const AString& source, size_t startIndex) const
{
  //a_Check if one is empty, then the diff must be zero
  if (!m_Length || !source.m_Length)
    return 0;
  
  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  if (!compare(source))
    return AConstant::npos;

  //a_Determine start and end
  size_t length = m_Length - startIndex;
  size_t end = (length > source.m_Length ? source.m_Length : length);
 
  //a_Iterate until difference is found
  size_t x;
  for (x = 0; x < end; ++x)
  {
    if (source.peek(x) != peek(x + startIndex))
      return x + startIndex;
  }

  return x;
}

size_t AString::_find(const char* pccSource, size_t length, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Check for empty source
  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);
  
  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);

  //a_See if we have enough data to search
  if (m_Length < length + startIndex)
    return AConstant::npos;
  
  const char* pccFound = strstr((mp_Buffer + startIndex), pccSource);

  if (pccFound)
    return pccFound - mp_Buffer;

  return AConstant::npos;
}

size_t AString::_find(char cSource, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  const char* pccFound = strchr((mp_Buffer + startIndex), cSource);

  if (pccFound)
    return pccFound - mp_Buffer;

  return AConstant::npos;
}

size_t AString::_rfind(const char* pccSource, size_t length, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Check for empty source
  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);

  if (startIndex == AConstant::npos)
    startIndex = m_Length - 1;
  else
  {
    //a_Index should be < length and if index == length that should be the '\x0'
    if (startIndex > m_Length)
      ATHROW(this, AException::IndexOutOfBounds);
  }

  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);

  //a_Check if the search will overrun
  if (startIndex < length)
    return AConstant::npos;
 
  AString strThis(*this), source(pccSource);
  strThis.reverse();
  source.reverse();

  size_t reverseStart = strThis.m_Length - startIndex - 1;
  const char* pccFound = strstr((strThis.mp_Buffer + reverseStart), source.mp_Buffer);

  if (pccFound)
  {
    size_t ret = (size_t)(pccFound - strThis.mp_Buffer);
    ret = strThis.m_Length - ret - source.m_Length;
    return ret;
  }

  return AConstant::npos;
}

size_t AString::_rfind(char cSource, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  const char* pccFound = NULL;

  //a_Check if we are searching the entire string
  if (startIndex == AConstant::npos)
  {
    pccFound = strrchr(mp_Buffer, cSource);

    if (pccFound)
      return pccFound - mp_Buffer;
  }
  else
  {
    //a_Index should be < length and if index == length that should be the '\x0'
    if (startIndex > m_Length)
      ATHROW(this, AException::IndexOutOfBounds);

    //a_We have a start address
    AString strThis;
    peek(strThis, 0, startIndex);
    pccFound = strrchr(strThis.mp_Buffer, cSource);

    if (pccFound)
      return pccFound - strThis.mp_Buffer;
  }

  return AConstant::npos;
}

size_t AString::_findNoCase(char cSource, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  const char* pccFound = mp_Buffer + startIndex;
  while (pccFound < mp_Buffer + m_Length)
  {
    if (tolower(*pccFound) == tolower(cSource))
      break;
    else
      pccFound++;
  }
  
  if (pccFound < mp_Buffer + m_Length)
    return pccFound - mp_Buffer;

  return AConstant::npos;
}

size_t AString::_findNoCase(const char* pccSource, size_t length, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Check for empty source
  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);
  
  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);

  //a_See if we have enough data to search
  if (m_Length < length + startIndex)
    return AConstant::npos;
  
  const char* pccFound = mp_Buffer + startIndex;
  while (pccFound < mp_Buffer + m_Length)
  {
    if (!_strnicmp(pccFound, pccSource, length))
      break;
    else
      pccFound++;
  }
  
  if (pccFound < mp_Buffer + m_Length)
    return pccFound - mp_Buffer;

  return AConstant::npos;
}

size_t AString::_rfindNoCase(char cSource, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  const char* pccFound = mp_Buffer;
  if (startIndex == AConstant::npos)
    pccFound += m_Length;
  else
  {
    //a_Index should be < length and if index == length that should be the '\x0'
    if (startIndex > m_Length)
      ATHROW(this, AException::IndexOutOfBounds);

    pccFound += startIndex;
  }

  while (pccFound >= mp_Buffer)
  {
    if (tolower(*pccFound) == tolower(cSource))
      break;
    else
      pccFound--;
  }

  if (pccFound >= mp_Buffer)
    return pccFound - mp_Buffer;

  return AConstant::npos;
}

size_t AString::_rfindNoCase(const char* pccSource, size_t length, size_t startIndex) const
{
  //a_Empty string
  if (!m_Length)
    return AConstant::npos;

  //a_Check for empty source
  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);

  if (startIndex == AConstant::npos)
    startIndex = m_Length - 1;
  else
  {
    //a_Index should be < length and if index == length that should be the '\x0'
    if (startIndex > m_Length)
      ATHROW(this, AException::IndexOutOfBounds);
  }

  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);

  //a_Check if the search will overrun
  if (startIndex < length)
    return AConstant::npos;
 
  AString strThis(*this), source(pccSource);
  strThis.reverse();
  source.reverse();

  size_t reverseStart = strThis.m_Length - startIndex - 1;
  const char* pccFound = strThis.mp_Buffer + reverseStart;
  
  while (
          (pccFound >= strThis.mp_Buffer) && 
          (pccFound < strThis.mp_Buffer + strThis.getSize())
        )
  {
    if (!_strnicmp(pccFound, source.mp_Buffer, length))
      break;
    else
      pccFound++;
  }

  if (pccFound < strThis.mp_Buffer + strThis.getSize())
  {
    size_t ret = pccFound - strThis.mp_Buffer;
    ret = strThis.m_Length - ret - source.m_Length;
    return ret;
  }

  return AConstant::npos;
}

//a_This method only guarantees that the size will be valid
//a_It sets the m_Length variable accordingly
//a_ clear() is the only method to modify the internal size and buffer allocation
void AString::_resize(size_t newBufferSize)
{
  if (newBufferSize < 0)
    ATHROW(this, AException::InvalidParameter);

  //a_Check is need to grow
  if (newBufferSize >= m_InternalBufferSize)
  {
    size_t allocationBuffer = m_InternalBufferSize;
    while (allocationBuffer <= newBufferSize)
      allocationBuffer += m_BufferIncrement;
    
    //a_Copy old content (if it exists)
    if (m_Length)
    {
      //a_Create new buffer
      char *pcBuffer = _allocate(allocationBuffer);

      //a_Copy old content
      memcpy(pcBuffer, mp_Buffer, m_Length + 1);

      //a_Swap
      char *pcTemp = mp_Buffer;
      mp_Buffer = pcBuffer;
      _deallocate(&pcTemp);

      m_Length = newBufferSize;
    }
    else
    {
      mp_Buffer = _allocate(allocationBuffer);
      mp_Buffer[0x0] = '\x0';

      m_Length = 0;
    }

    //a_New length
    m_InternalBufferSize = allocationBuffer;
  }
  else
    if (m_Length > newBufferSize)
      m_Length = newBufferSize;

  //a_Just reduce content
  mp_Buffer[newBufferSize] = '\x0';
}

u1 AString::peek(size_t index) const
{
  AASSERT(this, mp_Buffer);
  AASSERT(this, index < m_Length);

  return mp_Buffer[index];
}

size_t AString::peek(AOutputBuffer& bufDestination, size_t sourceIndex, size_t bytes) const
{
  //a_Copy to end
  if (bytes == AConstant::npos)
    bytes = m_Length - sourceIndex;

  //a_Nothing to do
  if (bytes == 0)
    return 0;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (sourceIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  //a_Check for read beyond end of content
  if (m_Length - sourceIndex - bytes < 0)
    ATHROW(this, AException::ReadBeyondEndOfBuffer);

  //a_Copy needed bytes
  bufDestination.append(mp_Buffer + sourceIndex, bytes);

  return bytes;
}

size_t AString::peekUntil(
  AOutputBuffer& target, 
  size_t sourceIndex, // = 0x0
  const AString& delimeters // = AConstant::ASTRING_WHITESPACE
) const
{
  if (sourceIndex >= m_Length)
    return AConstant::npos;
  
  size_t p = findOneOf(delimeters, sourceIndex);
  if (AConstant::npos != p) 
  {
    peek(target, sourceIndex, p - sourceIndex);
  }
  else
  {
    peek(target, sourceIndex);
  }
  return p;
}

size_t AString::peekUntil(AOutputBuffer& target, size_t sourceIndex, char delimeter) const
{
  if (sourceIndex >= m_Length)
    return AConstant::npos;
  
  size_t p = find(delimeter, sourceIndex);
  if (AConstant::npos != p) 
  {
    peek(target, sourceIndex, p - sourceIndex);
  }
  else
  {
    peek(target, sourceIndex);
  }
  return p;
}

u1& AString::use(size_t index)
{ 
  //a_Index should be < length and if index == length that should be the '\x0'
  if (index > m_Length || index < 0)
    ATHROW_EX(this, AException::IndexOutOfBounds, AString::fromSize_t(index));

  return (u1&)mp_Buffer[index]; 
}

u1 AString::get(size_t index)
{
  //a_Index should be < length and if index == length that should be the '\x0'
  if (index > m_Length || index < 0)
    ATHROW_EX(this, AException::IndexOutOfBounds, AString::fromSize_t(index));

  //a_First peek the value
  u1 uRet = mp_Buffer[index];

  //a_Now the ugly copy (which will copy the null as well)
  for (size_t x = index; x < m_Length; ++x)
    mp_Buffer[x] = mp_Buffer[x+1];

  //a_New size
  --m_Length;

  //a_NULL terminate
  mp_Buffer[m_Length] = '\x0';

  return uRet;
}

u1 AString::rget()
{
  if (m_Length < 1)
    ATHROW(this, AException::IndexOutOfBounds);

  //a_First peek the value
  u1 uRet = mp_Buffer[m_Length-1];

  //a_New size
  --m_Length;

  //a_NULL terminate
  mp_Buffer[m_Length] = '\x0';

  return uRet;
}

size_t AString::getUntilOneOf(
  AString& target,
  const AString& delimeters, // = AConstant::ASTRING_WHITESPACE
  bool removeDelimeters // = true
)
{
  if (isEmpty())
  {
    //a_Nothing left, result should be cleared also
    target.clear();
    return 0;
  }
  
  size_t p = findOneOf(delimeters);
  if (AConstant::npos == p) 
  {
    //a_No delimeters found
    target.assign(*this);
    clear();
    return AConstant::npos;
  }
  else
  {
    get(target, 0, p);
    if (removeDelimeters)
      stripLeading(delimeters);
  }
  return target.getSize();
}

size_t AString::getUntil(
  AString& target,
  const AString& pattern
)
{
  if (isEmpty())
  {
    //a_Nothing left, result should be cleared also
    target.clear();
    return 0;
  }
  
  size_t p = find(pattern);
  if (AConstant::npos == p) 
  {
    //a_No delimeters found
    target.assign(*this);
    clear();
    return AConstant::npos;
  }
  else
  {
    get(target, 0, p);
  }
  return target.getSize();
}

size_t AString::getUntil(
  AString& target, 
  char delimeter, 
  bool removeDelimeters // = true
)
{
  if (isEmpty())
  {
    //a_Nothing left, result should be cleared also
    target.clear();
    return 0;
  }
  
  size_t p = find(delimeter);
  if (AConstant::npos == p) 
  {
    //a_No delimeters found
    target.assign(*this);
    clear();
    return AConstant::npos;
  }
  else
  {
    get(target, 0, p);
    if (removeDelimeters)
      get();
  }
  return target.getSize();
}

void AString::removeUntilOneOf(
  const AString& delimeters, // = AConstant::ASTRING_WHITESPACE
  bool removeDelimeters // = true
)
{
  if (!isEmpty())
  {
    size_t p = findOneOf(delimeters);
    if (AConstant::npos == p) 
    {
      //a_No delimeters found
      clear();
    }
    else
    {
      _remove(p, 0);
      if (removeDelimeters)
        stripLeading(delimeters);
    }
  }
}

void AString::removeUntil(
  char delimeter, 
  bool removeDelimeters // = true
)
{
  if (!isEmpty())
  {
    size_t p = find(delimeter);
    if (AConstant::npos == p) 
    {
      //a_No delimeters found
      clear();
    }
    else
    {
      _remove(p+(removeDelimeters ? 1 : 0), 0);
    }
  }
}

size_t AString::get(AString& bufDestination, size_t sourceIndex, size_t bytes)
{
  //a_Clear destination
  bufDestination.clear();

  //a_Nothing to do
  if (!bytes)
    return 0;

  //a_Copy to end
  if (bytes == AConstant::npos)
    bytes = m_Length - sourceIndex;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (sourceIndex > m_Length || sourceIndex < 0)
    ATHROW(this, AException::IndexOutOfBounds);

  //a_And move
  if (m_Length - sourceIndex - bytes < 0)
    ATHROW(this, AException::ReadBeyondEndOfBuffer);

  //a_Copy needed bytes
  bufDestination.set(*this, 0, bytes, sourceIndex);

  //a_Now remove the data
  _remove(bytes, sourceIndex);

  return bytes;
}

void AString::_remove(size_t bytes, size_t sourceIndex)
{
  //a_Now the move
  size_t moveSize = m_Length - sourceIndex - bytes;
  if (moveSize < 0)
    ATHROW(this, AException::ReadBeyondEndOfBuffer);

  for (size_t x = 0; x < moveSize; ++x)
    mp_Buffer[sourceIndex + x] = mp_Buffer[sourceIndex + bytes + x];

  //a_Adjust size
  m_Length -= bytes;

  //a_NULL terminate
  mp_Buffer[m_Length] = '\x0';
}


void AString::set(u1 uByte, size_t index)
{
  AASSERT(this, mp_Buffer);

  //a_Index should be < length and if index == length that should be the '\x0'
  if (index > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  mp_Buffer[index] = uByte;
}

void AString::set(const AString& source, size_t destinationIndex)
{
  //a_Special case for simplicity of use
  set(source, destinationIndex, source.getSize(), 0x0);
}

void AString::set(const AString& source, size_t destinationIndex, size_t bytes, size_t sourceIndex)
{
  //a_Nothing to do
  if (!bytes)
    return;
  
  //a_Check is source has enough data
  if (source.getSize() < bytes + sourceIndex)
    ATHROW(this, AException::NotEnoughDataInSource);
  
  //a_Determine the size of the buffer after set
  size_t newSize = destinationIndex + bytes;

  //a_Resize the buffer
  _resize(newSize);

  //a_Now set the buffer
  memcpy(mp_Buffer + destinationIndex, (const char*)source.data() + sourceIndex, bytes);
  mp_Buffer[newSize] = '\x0';
  m_Length = newSize;
}

const void* AString::data() const
{
  return mp_Buffer;
}

void AString::clear(bool boolReleaseBuffer)
{
  //a_Length is the ultimate decider of content
  m_Length = 0;

  if (boolReleaseBuffer)
  {
    _deallocate(&mp_Buffer);
    m_InternalBufferSize = 0;
  }
}

size_t AString::getSize(bool boolPhysicalBuffer) const
{
  if (boolPhysicalBuffer)
    return m_InternalBufferSize;
  else
    return m_Length;
}

void AString::setSize(size_t newSize, u1 u1Pad)
{
  size_t oldSize = m_Length;
  _resize(newSize);
  
  //a_Clear/pad the buffer if size > 0
  if (newSize > 0)
  {
    //a_Pad the remaineder
    if (newSize > oldSize)
      memset(mp_Buffer + oldSize, u1Pad, newSize - oldSize);
    mp_Buffer[newSize] = '\x0';
    m_Length = newSize;
  }
  else
    m_Length = 0;
}

AString AString::operator+ (const AString& source) const
{
  AString strReturn(*this);
  if (source.m_Length > 0)
    strReturn.append(source.mp_Buffer, source.m_Length);
  return strReturn;
}

AString AString::operator+ (const char *pccSource) const
{
  AString strReturn(*this);
  if (pccSource)
    strReturn.append(pccSource, strlen(pccSource));
  return strReturn;
}

AString AString::operator+ (char cSource) const
{
  AString strReturn(*this);
  strReturn.append(&cSource, 1);
  return strReturn;
}

void AString::reverse()
{
  if (m_Length > 0)
  {
    AASSERT(this, mp_Buffer);
    char cTemp;
    size_t length = m_Length >> 0x1;
    for (size_t x = 0; x < length; ++x)
    {
      cTemp = mp_Buffer[x];
      mp_Buffer[x] = mp_Buffer[m_Length - x - 1];
      mp_Buffer[m_Length - x - 1] = cTemp;
    }
  }
}

void AString::makeLower()
{
  if (m_Length > 0x0)
  {
    AASSERT(this, mp_Buffer);
    for (size_t x = 0; x < m_Length; ++x)
    {
      mp_Buffer[x] = tolower(mp_Buffer[x]);
    }
  }
}

void AString::makeUpper()
{
  if (m_Length > 0x0)
  {
    AASSERT(this, mp_Buffer);
    for (u4 x = 0; x < m_Length; ++x)
    {
      mp_Buffer[x] = toupper(mp_Buffer[x]);
    }
  }
}

AString& AString::stripLeading(const AString& strDelimeters)
{
  //a_Nothing to do first character is not found in delimeters, or empty source
  if (
        (!m_Length) || 
        (strDelimeters._find(peek(0)) == AConstant::npos)
     )
    return *this;

  //a_Scan the original to see where to clip
  size_t pos = 0;
  while (
          (pos < m_Length) &&
          (strDelimeters._find(peek(pos)) != AConstant::npos)
        )
  {
    ++pos;
  }

  //a_iPos is where the first non matching character is, is it the end
  if (pos == m_Length)
  {		 
    clear();        //a_Clear all (quicker that remove all from front)
  }
  else
  {
    remove(pos);   //a_Remove from the front
  }

  return *this;
}

AString& AString::stripTrailing(const AString& strDelimeters)
{
  //a_Nothing to do last character is not found in delimeters, or empty source
  if ( 
       (!m_Length) ||
       (strDelimeters._find(peek(m_Length-1)) == AConstant::npos)
     )
    return *this;
  
  //a_Scan the original to see where to clip
  AASSERT(this, m_Length > 0);
  size_t pos = m_Length-1;

  while (strDelimeters._find(peek(pos)) != AConstant::npos)
  {
    if (!pos)
    {
      //a_Everything must go
      clear();
      return *this;
    }
    else
      --pos;
  }

  //a_Resize and truncate
  _resize(pos+1);

  return *this;
}

AString& AString::stripEntire(const AString& strDelimeters)
{
  size_t pos = 0;
  while ((pos = findOneOf(strDelimeters, pos)) != AConstant::npos)
  {
    remove(1, pos);
  }

  return *this;
}

size_t AString::findOneOf(const AString& strSet, size_t startIndex) const
{
  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  //a_Nothing to do first character is not found in delimeters, or empty source
  if (m_Length == 0x0)
    return AConstant::npos;

  //a_Scan the original to see where to clip
  size_t pos = startIndex;
  while (
          (pos < m_Length) &&
          (strSet._find(peek(pos)) == AConstant::npos)
        )
  {
    ++pos;
  }

  //a_iPos is where the first non matching character is, is it the end
  if (pos != m_Length)
    return pos;
  else
    return AConstant::npos;
}

size_t AString::findNotOneOf(const AString& strSet, size_t startIndex) const
{
  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  //a_Nothing to do first character is not found in delimeters, or empty source
  if (!m_Length)
    return AConstant::npos;

  //a_Scan the original to see where to clip
  size_t pos = startIndex;
  while (
          (pos < m_Length) &&
          (strSet._find(peek(pos)) != AConstant::npos)
        )
  {
    ++pos;
  }

  //a_pos is where the first non matching character is
  if (pos != m_Length)
    return pos;
  else
    return AConstant::npos;
}

size_t AString::rfindOneOf(const AString& strSet) const
{
  //a_Nothing to do first character is not found in delimeters, or empty source
  if (m_Length == 0x0)
    return AConstant::npos;

  //a_Scan the original to see where to clip
  size_t pos = m_Length-1;
  while (
          (pos > 0) &&
          (strSet._find(peek(pos)) == AConstant::npos)
        )
  {
    --pos;
  }

  if (pos)
    return pos;
  else
    return AConstant::npos;
}

size_t AString::rfindNotOneOf(const AString& strSet) const
{
  //a_Nothing to do first character is not found in delimeters, or empty source
  if (!m_Length)
    return AConstant::npos;

  //a_Scan the original to see where to clip
  size_t pos = m_Length - 1;
  while (
          (pos > 0) &&
          (strSet._find(peek(pos)) != AConstant::npos)
        )
  {
    --pos;
  }

  if (pos)
    return pos;
  else
    return AConstant::npos;
}

size_t AString::getHash(size_t upperLimit) const
{
  if (!m_Length)
    return 0;

  size_t hash = 5381;
  for (size_t x=0; x<m_Length; ++x)
  {
    // hash * 33 ^ curent_char 
    hash += (hash << 5);
    hash ^= mp_Buffer[x];
  }

  return (upperLimit == AConstant::npos ? hash : hash % upperLimit);
}

void AString::fromAFile(AFile& aFile)
{
  //a_Get length
  size_t length;
  aFile.read(length);

  clear();       //a_Default to empty string if 0 is the length

  //a_Read string data if length >0
  if (length > 0x0)
  {  
    AAutoBasicArrayPtr<char> pBuffer(new char[length + 1]);
    aFile.read(pBuffer.get(), length);
    *(pBuffer.get() + length) = '\x0';
    assign(pBuffer.get(), length);
  }
}

void AString::toAFile(AFile& aFile) const
{
  //a_Save length
  aFile.write(m_Length);
  
  //a_Write the string data if it exists
  if (m_Length > 0)
    aFile.write(mp_Buffer, m_Length);
}

void AString::peekFromFile(AFile& aFile)
{
  //a_Get length
  size_t length;
  aFile.peek(length);

  clear();       //a_Default to empty string if 0 is the length

  //a_Read string data if length >0
  if (length > 0)
  {  
    AAutoBasicArrayPtr<char> pBuffer(new char[length + 1]);
    aFile.peek(pBuffer.get(), length);
    *(pBuffer.get() + length) = '\x0';
    assign(pBuffer.get(), length);
  }
}

int AString::toInt() const 
{ 
  return (m_Length ? atoi(mp_Buffer) : 0);
}

s4 AString::toS4(int iBase) const
{ 
  char* pcEnd = "\x0";
  return (m_Length ? strtol(mp_Buffer, &pcEnd, iBase) : 0);
}

u4 AString::toU4(int iBase) const
{ 
  char* pcEnd = "\x0";
  return (m_Length ? strtoul(mp_Buffer, &pcEnd, iBase) : 0);
}

s8 AString::toS8(int iBase) const
{ 
  char* pcEnd = "\x0";
  return (m_Length ? _strtoi64(mp_Buffer, &pcEnd, iBase) : 0);
}

u8 AString::toU8(int iBase) const
{ 
  char* pcEnd = "\x0";
  return (m_Length ? _strtoui64(mp_Buffer, &pcEnd, iBase) : 0);
}

size_t AString::toSize_t(int iBase) const
{
  switch(sizeof(size_t))
  {
    case 8: return toU8(iBase);
    case 4: return toU4(iBase);
    default: ATHROW(NULL, AException::ProgrammingError);
  }
}

wchar_t AString::toUnicode() const
{
  size_t pos = 0;
  return toUnicode(pos);
}

wchar_t AString::toUnicode(size_t &startPos) const
{
  if (!m_Length || startPos == AConstant::npos)
    return 0;

  if (startPos >= m_Length)
  {
    startPos = AConstant::npos;
    return 0;
  }

  //a_Find the first occurence of UTF-8
  for (size_t posThis = startPos; posThis < m_Length; ++posThis)
  {
    if (~mp_Buffer[posThis] & 0x80)
    {
      //a_1 byte
      if (++startPos >= m_Length)
        startPos = AConstant::npos;

      return mp_Buffer[posThis];

    }
    else if (((mp_Buffer[posThis] >> 0x5) & 0x07) == 0x06)
    {
      //a_Check for bounds
      if (posThis + 0x1 >= m_Length)
      {
        startPos = AConstant::npos;
        return 0;
      }

      //a_2 byte
      if ((startPos += 0x2) >= m_Length)
        startPos = AConstant::npos;

      return  (  (((wchar_t)mp_Buffer[posThis] & 0x1F) << 0x6)
               + (((wchar_t)mp_Buffer[posThis + 0x1]) & 0x3F)
              );
      
  
    }
    else if (((mp_Buffer[posThis] >> 0x4) & 0x0F) == 0x0E)
    {
      //a_Check for bounds
      if (posThis + 0x2 >= m_Length)
      {
        startPos = AConstant::npos;
        return 0;
      }

      //a_3 byte
      if ((startPos += 0x3) >= m_Length)
        startPos = AConstant::npos;

      return (  ((((wchar_t)mp_Buffer[posThis]) & 0x0F) << 0xC)
               + ((((wchar_t)mp_Buffer[posThis + 0x1]) & 0x3F) << 0x6)
               + (((wchar_t)mp_Buffer[posThis + 0x2]) & 0x3F)
              );

    }
  }

  return 0;
}

void AString::_append(const char *pccSource, size_t length) 
{ 
  //a_Nothing to do
  if (!length)
    return;

  if (!pccSource)
    ATHROW(this, AException::SourceBufferIsNull);
  
  //a_If a length was not provided use strlen()
  if (length == AConstant::npos)
    length = strlen(pccSource);

  //a_Resize the buffer
  size_t oldLength = m_Length;
  _resize(oldLength + length);
  m_Length = oldLength + length;

  //aappend new content
  memcpy(mp_Buffer + oldLength, pccSource, length);
  mp_Buffer[m_Length] = '\x0';
}

void AString::appendN(int N, char cSource) 
{
  for (int n=0; n<N; ++n)
    append(&cSource, 1);
}

void AString::appendN(int N, const AString& source, size_t length) 
{ 
  if (length == AConstant::npos) length = source.m_Length;
  
  for (int n=0; n<N; ++n)
    append(source.mp_Buffer, length);
}

void AString::appendN(int N, const char *pccSource, size_t length) 
{ 
  if (pccSource)
  {
    if (length == AConstant::npos)
      length = strlen(pccSource);

    for (u2 n=0; n<N; ++n)
      append(pccSource, length);
  }
}

void AString::overwrite(size_t startIndex, size_t bytesToOverwrite, const AString& source, size_t sourceBytes)
{
  //a_Empty spurce or no bytes to copy from
  if (!m_Length || !sourceBytes)
    return;

  if (sourceBytes == AConstant::npos)
    sourceBytes = source.m_Length;

  //a_Source does not have enough bytes requested in replace
  if (sourceBytes > source.m_Length)
    ATHROW(this, AException::NotEnoughDataInSource);
  
  //a_Buffer offset and bytes to copy is greater than size
  if (startIndex + bytesToOverwrite > m_Length)
    ATHROW(this, AException::ReadBeyondEndOfBuffer);

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);
  
  ARope rope;
  rope.append(mp_Buffer, startIndex);
  rope.append(source.mp_Buffer, sourceBytes);
  rope.append(mp_Buffer + startIndex + bytesToOverwrite, m_Length - startIndex - bytesToOverwrite);

  clear();
  rope.emit(*this);
}

void AString::overwrite(size_t startIndex, const AString& source, size_t sourceBytes)
{
  //a_Empty spurce or no bytes to copy from
  if (!m_Length || !sourceBytes)
    return;

  if (sourceBytes == AConstant::npos)
    sourceBytes = source.m_Length;

  //a_Source does not have enough bytes requested in replace
  if (sourceBytes > source.m_Length)
    ATHROW(this, AException::NotEnoughDataInSource);
  
  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);
  
  //a_Make a copy that will be used as a copy source
  ARope rope;
  rope.append(mp_Buffer, startIndex);
  rope.append(source.mp_Buffer, sourceBytes);
  if (startIndex + sourceBytes < m_Length)
    rope.append(mp_Buffer + startIndex + sourceBytes, m_Length - startIndex - sourceBytes);
  
  clear();
  rope.emit(*this);
}

void AString::insert(size_t startIndex, const AString& source, size_t sourceStartIndex, size_t sourceBytes)
{
  //a_Empty source or no bytes to copy from
  if (!source.m_Length || !sourceBytes)
    return;

  //a_Same as append
  if (startIndex == AConstant::npos)
    startIndex = m_Length;

  //a_Paste entire source
  if (sourceBytes == AConstant::npos)
    sourceBytes = source.m_Length;

  //a_Source does not have enough bytes requested in replace
  if (sourceBytes > source.m_Length)
    ATHROW(this, AException::NotEnoughDataInSource);
  
  //a_Index should be < length and if index == length that should be the '\x0'
  if (sourceStartIndex > source.m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  if (startIndex == m_Length)
  {
    //a_Simple append
    if (sourceBytes > sourceStartIndex)
      append(source.mp_Buffer + sourceStartIndex, sourceBytes);
  }
  else
  {
    //a_Index should be < length and if index == length that should be the '\x0'
    if (startIndex > m_Length)
      ATHROW(this, AException::IndexOutOfBounds);

    //a_Make a copy that will be used as a copy source
    AString strThis(*this);

    _assign(strThis.mp_Buffer, startIndex);
    append(source.mp_Buffer + sourceStartIndex, sourceBytes - sourceStartIndex);
    append(strThis.mp_Buffer + startIndex, strThis.m_Length - startIndex);
  }
}

size_t AString::count(char cSource, size_t startIndex) const
{
  if (!m_Length)
    return 0;

  size_t count = 0x0;
  size_t pos = startIndex;
  while ((pos = find(cSource, pos)) != AConstant::npos)
  {
    ++pos;
    ++count;
  }
  return count;
}

char *AString::startUsingCharPtr(size_t neededSize)
{ 
  //a_Make space and return the char*
  if (AConstant::npos != neededSize)
  {
    _resize(neededSize);
    m_Length = neededSize;
  }

  return mp_Buffer;
}

size_t AString::stopUsingCharPtr(size_t newSize)
{
  //a_Add the trailing null
  if (newSize == AConstant::npos)
  {
    if (mp_Buffer)
      m_Length = strlen(mp_Buffer);
    else
      m_Length = 0;
  }
  else
  {
    if (m_InternalBufferSize > newSize)
    {
      m_Length = newSize;
      mp_Buffer[m_Length] = '\x0';
    }
    else
      ATHROW(this, AException::IndexOutOfBounds);
  }

  return m_Length;
}

void AString::assign(const AString& source, size_t length)
{ 
  if (source.m_Length == 0) 
  {
    clear();
  }
  else
  {
    if (length == AConstant::npos) length = source.m_Length;
    _assign(source.mp_Buffer, length);
  }
}

void AString::assign(const AEmittable& source)
{ 
  clear();
  source.emit(*this);
}

void AString::assignDoubleByte(const wchar_t* pwccSource, size_t wideCharCount)
{
  if (!pwccSource)
  {
    //a_NULL assign, release memory and clear
    if (m_Length) clear(true);
    return;
  }
  
  if (wideCharCount == AConstant::npos)
    wideCharCount = wcslen(pwccSource);

  _resize(wideCharCount * 0x3);    //a_Maximum possible length

  wchar_t wcWork;
  size_t pos = 0x0;
  for (size_t x = 0; x < wideCharCount; ++x)
  {
    wcWork = pwccSource[x];

    if (wcWork >= 0x0001 && wcWork < 0x0080)
    {
      //a_1 byte
      mp_Buffer[pos++] = (char)wcWork;
    }
    else if (!wcWork || (wcWork >= 0x0080 && wcWork < 0x0800) )
    {
      //a_2 byte (0x0 is encoded using 2 bytes)
      mp_Buffer[pos++] = (char)( 0xC0 | (wcWork >> 0x6) );
      mp_Buffer[pos++] = (char)( 0x80 | (wcWork & 0x001F) );
    }
    else if (wcWork >= 0x0800)
    {
      //a_3 byte
      mp_Buffer[pos++] = (char)( 0xE0 | (wcWork >> 0xC) );
      mp_Buffer[pos++] = (char)( 0x80 | ((wcWork >> 0x6) & 0x003F) );
      mp_Buffer[pos++] = (char)( 0x80 | (wcWork & 0x003F) );
    }
  }
  mp_Buffer[pos] = '\x0';
  m_Length = pos;
}

size_t AString::getUTF8Length() const
{
  size_t length = 0x0;
  for (size_t x = 0x0; x < m_Length; ++x)
  {
    if (~mp_Buffer[x] & 0x80) length++;                    //a_1 byte
    else if (mp_Buffer[x] & 0xC0) { length++; x++; }      //a_2 byte
    else if (mp_Buffer[x] & 0xE0) { length++; x += 0x2; } //a_3 byte
    else
      ATHROW(this, AException::InvalidData); 
  }
  return length;
}

wchar_t* AString::getDoubleByte(size_t wideCharCount) const
{
  if (!m_Length)
    return NULL;

  if (AConstant::npos == wideCharCount)
    wideCharCount = getUTF8Length();

  wchar_t* pwcRet = new wchar_t[wideCharCount + 1];
  
  size_t posThis = 0;  //a_Position into this
  size_t count = 0;    //a_Count of characters done so far
  while (count < wideCharCount)
  {
    //a_Check for bounds
    if (posThis >= m_Length)
      break;

    if (~mp_Buffer[posThis] & 0x80)
    {
      //a_1 byte
      pwcRet[count++] = mp_Buffer[posThis];

      ++posThis;
    }
    else if (((mp_Buffer[posThis] >> 0x5) & 0x07) == 0x06)
    {
      //a_Check for bounds
      if (posThis + 1 >= m_Length)
        break;

      //a_2 byte
      pwcRet[count++] = (  (((wchar_t)mp_Buffer[posThis] & 0x1F) << 0x6)
                         + (((wchar_t)mp_Buffer[posThis + 0x1]) & 0x3F)
                        );
    
      posThis += 2;
    }
    else if (((mp_Buffer[posThis] >> 0x4) & 0x0F) == 0x0E)
    {
      //a_Check for bounds
      if (posThis + 2 >= m_Length)
        break;

      //a_2 byte
      pwcRet[count++] = (  ((((wchar_t)mp_Buffer[posThis]) & 0x0F) << 0xC)
                         + ((((wchar_t)mp_Buffer[posThis + 0x1]) & 0x3F) << 0x6)
                         + (((wchar_t)mp_Buffer[posThis + 0x2]) & 0x3F)
                        );

      posThis += 0x3;
    }
  }

  pwcRet[wideCharCount] = 0;
  return pwcRet;
}

AString AString::fromUnicode(wchar_t wcValue)
{
  AString str;
  if (wcValue >= 0x0001 && wcValue < 0x0080)
  {
    //a_1 byte
    str = (char)wcValue;
  }
  else if (!wcValue || (wcValue >= 0x0080 && wcValue < 0x0800) )
  {
    //a_2 byte (0x0 is encoded using 2 bytes)
    str  = (char)( 0xC0 | (wcValue >> 0x6) );
    str += (char)( 0x80 | (wcValue & 0x001F) );
  }
  else if (wcValue >= 0x0800)
  {
    //a_3 byte
    str  = (char)( 0xE0 | (wcValue >> 0xC) );
    str += (char)( 0x80 | ((wcValue >> 0x6) & 0x003F) );
    str += (char)( 0x80 | (wcValue & 0x003F) );
  }

  return str;
}

void AString::fill(u1 uByte)
{
	memset(mp_Buffer, uByte, m_Length);
	mp_Buffer[m_Length] = '\x0';
}

void AString::replace(
  char source, 
  char target, 
  size_t startIndex,  // = 0
  size_t numberToReplace  // = AConstant::MAX_SIZE_T
)
{
  //a_Do nothing if index out of bounds or empty string
  if (!m_Length)
    return;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  for (size_t i = startIndex; i < m_Length && numberToReplace; ++i)
  {
    if (mp_Buffer[i] == source)
    {
      mp_Buffer[i] = target;
      --numberToReplace;
    }
  }
}

void AString::replace(
  const AString& source, 
  const AString& target, 
  size_t startIndex,    // = 0
  size_t numberToReplace    // = AConstant::MAX_SIZE_T
)
{
  //a_Do nothing if index out of bounds or empty string
  if (!m_Length)
    return;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);
  
  size_t count = 0;
  size_t pos = find(source, startIndex);
  while (AConstant::npos != pos && count < numberToReplace)
  {
    if (source.getSize() == target.getSize())
    {
      memcpy(mp_Buffer + pos, target.mp_Buffer, target.m_Length);
    }
    else
    {
      ARope rope;
      AString str;

      peek(str, 0, pos);
      rope.append(str);
      rope.append(target);
      str.clear();
      peek(str, pos + source.getSize());
      rope.append(str);

      clear();
      rope.emit(*this);
    }

    pos = find(source, pos);
    ++count;
  }
}

void AString::truncateAt(
  char delimiter, 
  size_t startIndex // = 0
)
{
  //a_Do nothing if index out of bounds or empty string
  if (!m_Length)
    return;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  char* pccFound = strchr((mp_Buffer + startIndex), delimiter);
  if (pccFound)
    setSize(pccFound - mp_Buffer);
}

void AString::truncateAtOneOf(
  const AString& strSet, 
  size_t startIndex // = 0x0
)
{
  //a_Do nothing if index out of bounds or empty string
  if (!m_Length)
    return;

  //a_Index should be < length and if index == length that should be the '\x0'
  if (startIndex > m_Length)
    ATHROW(this, AException::IndexOutOfBounds);

  for (size_t i = startIndex; i < m_Length; ++i)
  {
    if (strSet.find(mp_Buffer[i]) != AConstant::npos)
    {
      setSize(i);
      return;
    }
  }
}

void AString::emit(AOutputBuffer& target) const
{ 
  if (mp_Buffer && m_Length > 0)
    target.append(mp_Buffer, m_Length);
}

void AString::emit(AXmlElement& target) const
{
  target.addAttribute(ASW("class",5), ASW("AString",7));
  target.addData(*this, AXmlData::CDataDirect);
}

size_t AString::split(
  LIST_AString& slResult, 
  char delimeter,         // = ' '
  const AString& strip,   // = AConstant::ASTRING_EMPTY
  bool bKeepEmpty         // = false
) const
{
  if (!m_Length)
    return 0;

  size_t foundCount = 0;
  AString str;
  size_t pos = 0;

  while (AConstant::npos != pos)
  {
    if (AConstant::npos != (pos = peekUntil(str, pos, delimeter)))
      ++pos;

    if (!str.isEmpty() || bKeepEmpty)
    {
      if (!strip.isEmpty())
        str.stripEntire(strip);

      slResult.push_back(str);
      str.clear();
    }
  }

  return foundCount;
}

size_t AString::split(
  VECTOR_AString& slResult,
  char delimeter,         // = ' '
  const AString& strip,   // = AConstant::ASTRING_EMPTY
  bool bKeepEmpty         // = false
) const
{
  if (!m_Length)
    return 0;

  size_t foundCount = 0;
  slResult.reserve(count(delimeter));   //a_This will prevent the reallocations if vector is not large enough
  AString str;
  size_t pos = 0;

  while (AConstant::npos != pos)
  {
    if (AConstant::npos != (pos = peekUntil(str, pos, delimeter)))
      ++pos;

    if (!str.isEmpty() || bKeepEmpty)
    {
      if (!strip.isEmpty())
        str.stripEntire(strip);

      slResult.push_back(str);
    }

    str.clear();
  }

  return foundCount;
}

u1 AString::at(size_t index) const
{
  return peek(index);
}

u1 AString::last() const
{
  if (m_Length > 0)
    return peek(m_Length-1);
  else
    ATHROW(this, AException::IndexOutOfBounds);
}

u1 AString::at(size_t index, u1 u1Default) const
{
  if (!mp_Buffer || index > m_Length)
    return u1Default;
  else
    return mp_Buffer[index];
}

u1 AString::operator[](size_t index) const
{ 
  return peek(index);
}

u1& AString::operator[](size_t index)
{ 
  return use(index);
}

void AString::remove(
  size_t length,        // = 1
  size_t sourceOffset // = 0
)
{
  _remove(length, sourceOffset); 
}

void AString::rremove(
  size_t length // = 0x1
)
{
  AASSERT(this, length <= m_Length);
  setSize(m_Length - length);
}

void AString::rremoveUntilOneOf(
  const AString& delimeters, //= AConstant::ASTRING_WHITESPACE
  bool removeDelimeters      //= true
)
{
  size_t pos = rfindOneOf(delimeters);
  if (AConstant::npos != pos)
  {
    AASSERT(this, pos - (removeDelimeters ? 1 : 0) >= 0);
    setSize(pos - (removeDelimeters ? 1 : 0));
  }
}

void AString::rremoveUntil(
  char delimeter, 
  bool removeDelimeter       // = true
)
{
  size_t pos = rfind(delimeter);
  if (AConstant::npos != pos)
  {
    AASSERT(this, pos - (removeDelimeter ? 0 : -1) >= 0);
    setSize(pos - (removeDelimeter ? 0 : -1));
  }
}

int AString::compare(char cSource) const
{ 
  return ((m_Length == 0x1 && mp_Buffer[0x0] == cSource) ? 0 : 1);
}

int AString::compare(const AString& source) const
{ 
  return _compare(source.mp_Buffer, source.m_Length);
}

int AString::compareNoCase(const AString& source) const
{ 
  return _compare(source.mp_Buffer, source.m_Length, true);
}

bool AString::equals(const AString& source) const
{
  return (_compare(source.mp_Buffer, source.m_Length) ? false : true);
}

bool AString::equalsNoCase(const AString& source) const
{ 
  return (_compare(source.mp_Buffer, source.m_Length, true) ? false : true);
}

bool AString::equals(
  const char *pccSource, 
  size_t length // = AConstant::npos
) const
{
  if (!pccSource)
    ATHROW(this, AException::InvalidParameter);

  if (AConstant::npos == length)
    length = strlen(pccSource);

  return (_compare(pccSource, length) ? false : true);
}

bool AString::equalsNoCase(
  const char *pccSource, 
  size_t length // = AConstant::npos
) const
{
  if (!pccSource)
    ATHROW(this, AException::InvalidParameter);

  if (AConstant::npos == length)
    length = strlen(pccSource);

  return (_compare(pccSource, length, true) ? false : true);
}

bool AString::isEmpty() const 
{ 
  return (m_Length ? false : true); 
}

void AString::justifyRight(
  size_t size,
  char pad //= ' '
)
{
  if (size <= m_Length)
    return;

  size_t oldSize = m_Length;
  _resize(size);   //a_Allocate space
  
  size_t t;
  for (t=0; t < oldSize; ++t)
    mp_Buffer[size-t-1] = mp_Buffer[oldSize-t-1];

  for (t=0; t < size-oldSize; ++t)
    mp_Buffer[t] = pad;

  m_Length = size;
}

void AString::justifyCenter(
  size_t size,
  char pad      // = ' '
)
{
  if (size <= m_Length)
    return;

  size_t oldSize = m_Length;
  _resize(size);   //a_Allocate space
  
  size_t diffR = (size - oldSize) >> 1;    //a_right pad
  size_t diffL = size - oldSize - diffR;   //a_left pad
  
  size_t t;
  for (t=0; t < oldSize; ++t)
    mp_Buffer[size-diffR-t-1] = mp_Buffer[oldSize-t-1];

  //a_Pad front
  for (t=0; t < diffL; ++t)
    mp_Buffer[t] = pad;

  //a_Pad back
  for (t=0; t < diffR; ++t)
    mp_Buffer[size-t-1] = pad;

  m_Length = size;
}

void AString::justifyLeft(
  size_t size,
  char pad      // = ' '
)
{
  if (size <= m_Length)
    return;

  setSize(size, pad);
}

AString& AString::operator+= (const AString& source)
{ 
  append(source.mp_Buffer, source.m_Length); 
  return *this; 
}

AString& AString::operator+= (const char *pccSource)
{ 
  append(pccSource, AConstant::npos); 
  return *this; 
}

AString& AString::operator+= (const char cSource)        
{ 
  append(&cSource, 1); 
  return *this; 
}
AString& AString::operator= (const char * pccSource)
{ 
  assign(pccSource, AConstant::npos); 
  return *this; 
}

AString& AString::operator= (const char cSource)         
{ 
  assign(&cSource, 1); 
  return *this; 
}

size_t AString::find(const AString& source, size_t startIndex) const 
{ 
  return _find(source.mp_Buffer, source.m_Length, startIndex); 
}

size_t AString::find(const char *pccSource, size_t startIndex) const 
{ 
  return _find(pccSource, AConstant::npos, startIndex);
}

size_t AString::find(char cSource, size_t startIndex) const 
{ 
  return _find(cSource, startIndex); 
}

size_t AString::findNoCase(const AString& source, size_t startIndex) const 
{ 
  return _findNoCase(source.mp_Buffer, source.m_Length, startIndex); 
}

size_t AString::findNoCase(const char *pccSource, size_t startIndex) const 
{ 
  return _findNoCase(pccSource, AConstant::npos, startIndex);
}

size_t AString::findNoCase(char cSource, size_t startIndex) const 
{ 
  return _findNoCase(cSource, startIndex); 
}

size_t AString::rfind(
  const AString& source, 
  size_t startIndex // = 0
) const
{ 
  return _rfind(source.mp_Buffer, source.m_Length, startIndex);
}

size_t AString::rfind(
  const char *pccSource, 
  size_t sourceLength, // = AConstant::npos
  size_t startIndex    // = AConstant::npos
) const
{ 
  return _rfind(pccSource, sourceLength, startIndex);
}

size_t AString::rfind(
  char cSource, 
  size_t startIndex // = AConstant::npos
) const
{ 
  return _rfind(cSource, startIndex);
}

size_t AString::rfindNoCase(
  const AString& source, 
  size_t startIndex // = AConstant::npos
) const
{ 
  return _rfindNoCase(source.mp_Buffer, source.m_Length, startIndex);
}

size_t AString::rfindNoCase(
  const char *pccSource, 
  size_t startIndex // = AConstant::npos
) const
{ 
  return _rfindNoCase(pccSource, AConstant::npos, startIndex);
}

size_t AString::rfindNoCase(
  char cSource, 
  size_t startIndex // = AConstant::npos
) const
{ 
  return _rfindNoCase(cSource, startIndex);
}

u2 AString::getBufferIncrementSize() const
{ 
  return m_BufferIncrement; 
}

void AString::setBufferIncrementSize(u2 newIncrement)
{ 
  m_BufferIncrement = newIncrement; 
}

void AString::reserve(size_t size) 
{ 
  _resize(size);
}

void AString::reserveMoreSpace(size_t moreSpace)
{ 
  _resize(getSize() + moreSpace);
}

bool AString::operator< (const AString& strSource) const
{
  return (_compare(strSource.mp_Buffer, strSource.m_Length) <  0 ? true : false);
}

bool AString::operator> (const AString& strSource) const
{ 
  return (_compare(strSource.mp_Buffer, strSource.m_Length) >  0 ? true : false); 
}

bool AString::operator<= (const AString& strSource) const
{ 
  return (_compare(strSource.mp_Buffer, strSource.m_Length) <= 0 ? true : false);
}

bool AString::operator>= (const AString& strSource) const
{ 
  return (_compare(strSource.mp_Buffer, strSource.m_Length) >= 0 ? true : false);
}

bool AString::operator== (const AString& strSource) const
{ 
  return (_compare(strSource.mp_Buffer, strSource.m_Length) == 0 ? true : false);
}

bool AString::operator!= (const AString& strSource) const
{ 
  return (_compare(strSource.mp_Buffer, strSource.m_Length) != 0 ? true : false);
}

bool AString::operator< (const char *pccSource) const
{ 
  return (_compare(pccSource, AConstant::npos) <  0 ? true : false);
}

bool AString::operator> (const char *pccSource) const 
{ 
  return (_compare(pccSource, AConstant::npos) >  0 ? true : false);
}

bool AString::operator<= (const char *pccSource) const
{ 
  return (_compare(pccSource, AConstant::npos) <= 0 ? true : false); 
}

bool AString::operator>= (const char *pccSource) const 
{ 
  return (_compare(pccSource, AConstant::npos) >= 0 ? true : false);
}

bool AString::operator== (const char *pccSource) const 
{ 
  return (_compare(pccSource, AConstant::npos) == 0 ? true : false);
}

bool AString::operator!= (const char *pccSource) const 
{ 
  return (_compare(pccSource, AConstant::npos) != 0 ? true : false);
}

void AString::assign(char cSource)
{
  _assign(&cSource, 1);
}

void AString::assign(const char *pccSource, size_t length /* = AConstant::npos */)
{ 
  _assign(pccSource, length); 
}

void AString::assign(const unsigned char *pccSource, size_t length /* = AConstant::npos */)
{ 
  _assign((const char *)pccSource, length); 
}
