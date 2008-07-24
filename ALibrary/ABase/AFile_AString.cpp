/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFile_AString.hpp"

void AFile_AString::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReadPos=" << m_ReadPos << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_WritePos=" << m_WritePos << std::endl;
  m_Data.debugDump(os, indent+1);
  AFile::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AFile_AString::setReadPos(size_t pos)  { m_ReadPos = pos; }
size_t   AFile_AString::getReadPos() const  { return m_ReadPos; } 
void AFile_AString::setWritePos(size_t pos) { m_WritePos = pos; }
size_t   AFile_AString::getWritePos() const { return m_WritePos; }
void AFile_AString::reset() { m_ReadPos = 0; m_WritePos = m_Data.getSize(); }
void AFile_AString::clear() { m_ReadPos = 0; m_WritePos = 0; m_Data.clear(); }

AFile_AString::AFile_AString() :
  m_ReadPos(0),
  m_WritePos(0)
{
}

AFile_AString::AFile_AString(size_t initial, u2 increment) :
  m_Data(initial, increment),
  m_ReadPos(0),
  m_WritePos(0)
{
}

AFile_AString::AFile_AString(const AString& data) :
  m_Data(data),
  m_ReadPos(0)
{
  m_WritePos = m_Data.getSize();
}

AFile_AString::AFile_AString(const AFile_AString& that) :
  m_Data(that.m_Data),
  m_ReadPos(that.m_ReadPos),
  m_WritePos(that.m_WritePos)
{
}

size_t AFile_AString::_read(void *pData, size_t u4Len)
{
  if (m_ReadPos >= m_Data.getSize())
    return 0;  //a_EOF

  char *pcData = static_cast<char *>(pData);
  if (m_ReadPos + u4Len >= m_Data.getSize())
    u4Len = m_Data.getSize() - m_ReadPos;

  memcpy(pcData, m_Data.c_str() + m_ReadPos, u4Len);

  m_ReadPos += u4Len;

  return u4Len;
}

size_t AFile_AString::_write(const void *pData, size_t u4Len)
{
  if (m_WritePos > m_Data.getSize())
    m_WritePos = m_Data.getSize();  //a_Position beyond the end of the buffer becomes the end

  const char *pccData = static_cast<const char *>(pData);

  if (m_WritePos < m_Data.getSize())
    m_Data.setSize(m_WritePos);  //a_Truncate buffer to write pos

  //a_Write position at end, append
  m_Data.append(pccData, u4Len);
  m_WritePos = m_Data.getSize();

  return u4Len;
}

const AString& AFile_AString::getAString() const
{
  return m_Data; 
}

AString& AFile_AString::useAString()
{ 
  return m_Data;
}

bool AFile_AString::_isNotEof()
{
  return (m_ReadPos < m_Data.getSize());
}

void AFile_AString::emit(AOutputBuffer& target) const
{
  m_Data.emit(target);
}

size_t AFile_AString::getSize() const
{
  return m_Data.getSize();
}

void AFile_AString::rewind()
{
  m_ReadPos = 0;
}

size_t AFile_AString::access(
  AOutputBuffer& target, 
  size_t index,          // = 0 
  size_t bytes           // = AConstant::npos
) const
{
  return m_Data.peek(target, index, bytes);
}

