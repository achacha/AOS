#include "pchABase.hpp"
#pragma hdrstop

#include "AFile.hpp"
#include "AException.hpp"
#include "ARope.hpp"

#ifdef __DEBUG_DUMP__
void AFile::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFile @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReadBlock=" << m_ReadBlock << std::endl;
  m_LookaheadBuffer.debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AFile::AFile() :
  m_ReadBlock(AFile::DefaultReadBlock)
{
}

bool AFile::isNotEof()
{
  char c;
  if (_isNotEof())
    return true;                        //a_Derived file is not at EOF
  else
    return (peek(c) ? true : false);    //a_Peek still has data so not yet EOF
}

size_t AFile::readLine(
  AString &strRead,
  size_t maxBytes,   // = AConstant::npos
  bool treatEofAsEol // = true
)
{
  //a_AString::npos return means not found, 0 means empty line, maxBytes means read to max
  size_t originalSize = strRead.getSize();
  size_t ret = m_LookaheadBuffer.popFrontUntil(strRead, '\n', true, true, maxBytes);

  while (AConstant::npos == ret && _isNotEof())
  {
    size_t read = _readBlockIntoLookahead();
    if (!read || AConstant::npos == read)
    {
      if (treatEofAsEol && m_LookaheadBuffer.getSize() > 0)
      {
        //a_Nothing left to read, so will read to EOF
        ret = m_LookaheadBuffer.getSize();
        m_LookaheadBuffer.emit(strRead);
        m_LookaheadBuffer.clear();
        return ret;
      }
      else
        return AConstant::npos;
    }
    ret = m_LookaheadBuffer.popFrontUntil(strRead, '\n', true, true, maxBytes);

    //a_Lookahead is empty, wait short time and try reading again
    //if (0 == ret)
    //{
    //  //a_No delimeter, get rest of data
    //  m_LookaheadBuffer.emit(strRead);
    //  m_LookaheadBuffer.clear();

    //  return strRead.getSize();
    //}
  }

  if (
    AConstant::npos != ret 
    && ret > 0 
    && strRead.getSize() > originalSize 
    && strRead.last() == '\r'
  )
  {
    //a_Remove CR that was before LF
    strRead.rremove();
  }
  
  //a_Check if blank line read
  if (!ret && originalSize == strRead.getSize())
  {
    return 0;
  }

  if (originalSize == strRead.getSize() && !_isNotEof())
    return AConstant::npos;
  else
    return strRead.getSize();
}

size_t AFile::peekLine(
  AString &strPeek, 
  size_t maxBytes,    // = AConstant::npos
  bool treatEofAsEol  // = true
)
{
  size_t ret = m_LookaheadBuffer.peekFrontUntil(strPeek, '\n', maxBytes);

  while (AConstant::npos == ret)
  {
    if (!_readBlockIntoLookahead())
    {
      if (treatEofAsEol)
        ret = m_LookaheadBuffer.peekFront(strPeek, maxBytes);
      else
        ret = AConstant::npos;
    }
    else
    {
      ret = m_LookaheadBuffer.peekFrontUntil(strPeek, '\r', maxBytes);
    }
  }

  return ret;
}

size_t AFile::skipLine(
  size_t maxBytes,    // = AConstant::npos
  bool treatEofAsEol  // = true
)
{
  size_t ret = m_LookaheadBuffer.removeFrontUntil('\n', true, maxBytes);

  while (AConstant::npos == ret)
  {
    if (!_readBlockIntoLookahead())
    {
      if (treatEofAsEol)
      {
        ret = m_LookaheadBuffer.removeFront(maxBytes);
      }
      else
        ret = AConstant::npos;
    }
    else
    {
      ret = m_LookaheadBuffer.removeFrontUntil('\n', true, maxBytes);
    }
  }

  return ret;
}

size_t AFile::readUntilOneOf(
  AString &strRead,
  const AString &strDelimeters, 
  bool boolRemoveDelimiter /* = true */,
  bool boolDiscardDelimiter /* = true */)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.popFrontUntilOneOf(strRead, strDelimeters, boolRemoveDelimiter, boolDiscardDelimiter)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::peekUntilOneOf(AString &strPeek, const AString &strDelimeters)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.peekFrontUntilOneOf(strPeek, strDelimeters)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }
  return ret;
}

size_t AFile::skipUntilOneOf(const AString &strDelimeters, bool boolDiscardDelimeter /* = true */)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.removeFrontUntilOneOf(strDelimeters, boolDiscardDelimeter)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::skipUntilOneOf(char delimeter, bool boolDiscardDelimeter /* = true */)
{
  size_t ret = 0;
  AString strDelim(delimeter);
  while (AConstant::npos == (ret = m_LookaheadBuffer.removeFrontUntilOneOf(strDelim, boolDiscardDelimeter)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::peekUntil(AString& strPeek, const AString& strPattern)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.peekFrontUntil(strPeek, strPattern)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }
  return ret;
}

size_t AFile::readUntil(
  AString& strRead, 
  const AString& strPattern, 
  bool boolRemovePattern /* = true */, 
  bool boolDiscardPattern /* = true */)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.popFrontUntil(strRead, strPattern, boolRemovePattern, boolDiscardPattern)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::skipUntil(const AString& strPattern, bool boolDiscardPattern /* = true*/)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.removeFrontUntil(strPattern, boolDiscardPattern)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::skipOver(const AString& strDelimeters /* = AString::sstr_WhiteSpace */)
{
  char c;
  if (!peek(c))
    return AConstant::npos;

  size_t ret = 0;
  while (strDelimeters.find(c) != AConstant::npos)
  {
    read(c);
    ++ret;
    if (!peek(c))
      return AConstant::npos;
  }

  return ret;
}

size_t AFile::find(const AString &strPattern)
{
  size_t ret = 0;
  while (AConstant::npos == (ret = m_LookaheadBuffer.find(strPattern)))
  {
    if (!_readBlockIntoLookahead())
      return AConstant::npos;
  }
  return ret;
}

size_t AFile::writeLine(const AString& line, const AString& strEOL /* = AString::sstr_EOL*/)
{
  size_t written = 0;
  if (line.getSize() > 0)
  {
    written = _write(line.data(), line.getSize());
  }

  written += _write(strEOL.data(), strEOL.getSize());
  return written;
}

size_t AFile::writeEOL(const AString& strEOL /* = AString::sstr_EOL*/)
{
  size_t written = _write(strEOL.data(), strEOL.getSize());
  return written;
}

size_t AFile::write(const void *pcvBuffer, size_t length)
{
  return _write(pcvBuffer, length);
}

size_t AFile::write(AFile& sourceFile)
{
  const int BUFFER_SIZE = 4096;
  AString buffer(BUFFER_SIZE, 256);
  size_t totalBytesRead = 0;
  size_t bytesRead = 0;
  do 
  {
    bytesRead = sourceFile.read(buffer, BUFFER_SIZE);
    totalBytesRead += bytesRead;
    write(buffer);
    buffer.clear();
  }
  while(bytesRead == BUFFER_SIZE);

  return totalBytesRead;
}

size_t AFile::read(AString& str, size_t length)
{
  size_t initialSize = str.getSize();
  size_t bytesread = read(str.startUsingCharPtr(length+initialSize)+initialSize, length);
  AASSERT(this, AConstant::npos != bytesread);
  str.stopUsingCharPtr(initialSize + bytesread);
  return bytesread;
}

size_t AFile::peek(AString& str, size_t length)
{
  size_t initialSize = str.getSize();
  size_t bytesread = peek(str.startUsingCharPtr(length+initialSize)+initialSize, length);
  AASSERT(this, AConstant::npos != bytesread);
  str.stopUsingCharPtr(initialSize + bytesread);
  return bytesread;
}

size_t AFile::skip(size_t length)
{
  char tempBuffer[128];
  size_t bytesread;
  size_t totalbytesread = 0;
  while (length > 0)
  {
    bytesread = read(tempBuffer, (length > 128 ? 128 : length));
    if (AConstant::npos == bytesread || !bytesread)
      break;
    
    AASSERT(this, AConstant::npos != bytesread);
    length -= bytesread;
    totalbytesread += bytesread;
  }
  return totalbytesread;
}

size_t AFile::write(const AString& line)
{
  return _write(line.data(), line.getSize());
}

size_t AFile::read(ARope& rope, size_t length)
{
  return rope.fromAFile(*this, length);
}

size_t AFile::write(const ARope& rope)
{
  return rope.toAFile(*this);
}

size_t AFile::peekUntilEOF(AOutputBuffer& target)
{
  //a_Read everything into lookahead buffer for peek
  size_t ret = _readBlockIntoLookahead();
  while (ret > 0)
  {
    ret = _readBlockIntoLookahead();
  }

  m_LookaheadBuffer.emit(target);

  return m_LookaheadBuffer.getSize();
}

size_t AFile::readUntilEOF(AOutputBuffer& target)
{
  size_t bytesRead = m_LookaheadBuffer.getSize();
  if (bytesRead > 0)
  {
    m_LookaheadBuffer.emit(target);
    m_LookaheadBuffer.clear();
  }
  
  //a_Read
  size_t ret = _readBlockIntoLookahead();
  if (ret > 0)
  {
    do 
    {
      bytesRead += ret;
      m_LookaheadBuffer.emit(target);
      m_LookaheadBuffer.clear();
      ret = _readBlockIntoLookahead();
    }
    while (ret == m_ReadBlock);

    if (ret > 0)
    {
      bytesRead += ret;
      m_LookaheadBuffer.emit(target);
      m_LookaheadBuffer.clear();
    }
  }

  return bytesRead;
}

size_t AFile::_readBlockIntoLookahead()
{
  AString str;
  char *p = str.startUsingCharPtr(m_ReadBlock);
  
  size_t bytesRead = _read(p, m_ReadBlock);
  if (bytesRead > 0)
    m_LookaheadBuffer.pushBack(p, bytesRead);

  return bytesRead;
}

size_t AFile::read(void *pTarget, size_t bytesLeft)
{
  size_t inLookahead = m_LookaheadBuffer.getSize();
  if (inLookahead > bytesLeft)
  {
    //a_Have all the data
    m_LookaheadBuffer.popFront((char *)pTarget, bytesLeft);
    return bytesLeft;
  }
  else
  {
    //a_Read what we have so far in the lookahead
    if (inLookahead)
    {
      m_LookaheadBuffer.popFront((char *)pTarget, inLookahead);
      bytesLeft -= inLookahead;
    }
  }

  //a_Read from channel
  size_t bytesRead = 0;
  while (m_LookaheadBuffer.getSize() < bytesLeft)
  {
    size_t bytesReadPass = _readBlockIntoLookahead();
    
    // npos returned is WOULDBLOCK is the case
    if (!bytesReadPass || AConstant::npos == bytesReadPass)
      break;

    bytesRead += bytesReadPass;
  }
  if (bytesRead > bytesLeft)
    bytesRead = bytesLeft;

  if (bytesRead)
    m_LookaheadBuffer.popFront((char *)pTarget + inLookahead, bytesRead);
  
  return bytesRead + inLookahead;
}

size_t AFile::peek(void *pTarget, size_t bytesLeft)
{
  size_t inLookahead = m_LookaheadBuffer.getSize();
  if (inLookahead > bytesLeft)
  {
    //a_Have all the data
    m_LookaheadBuffer.peekFront((char *)pTarget, bytesLeft);
    return bytesLeft;
  }
  else
  {
    //a_Read file memory until we have what we need
    size_t bytesRead = 0;
    while (m_LookaheadBuffer.getSize() < bytesLeft)
    {
      size_t bytesReadPass = _readBlockIntoLookahead();
      bytesRead += bytesReadPass;
      if (!bytesReadPass)   //a_Blocking or out of data same for peek
        break;
    }

    bytesRead = m_LookaheadBuffer.getSize();
    if (bytesRead > bytesLeft)
      bytesRead = bytesLeft;

    if (bytesRead)
      m_LookaheadBuffer.peekFront((char *)pTarget, bytesRead);

    return bytesRead;
  }
}

size_t AFile::_writeNetworkOrder(const u1* pData, size_t uLength)
{
  size_t ret = 0x0;
  
  switch(uLength)
  {
    case 0x1:
      ret = write(pData, 0x1);
    break;

    case 0x2:
      ret = _writeNetworkOrdersize_t(pData);
    break;

    case 0x4:
      ret = _writeNetworkOrdersize_t(pData);
    break;

    case 0x8:
      ret = _writeNetworkOrderU8(pData);
    break;

    default:
      ATHROW(this, AException::InvalidBinaryLength);
  }

  return ret;
}

size_t AFile::_readNetworkOrder(u1* pData, size_t uLength)
{
  size_t ret = 0x0;

  switch(uLength)
  {
    case 0x1:
      ret = read(pData, 0x1);
    break;

    case 0x2:
      ret = _readNetworkOrderU2(pData);
    break;

    case 0x4:
      ret = _readNetworkOrdersize_t(pData);
    break;

    case 0x8:
      ret = _readNetworkOrderU8(pData);
    break;

    default:
      ATHROW(this, AException::InvalidBinaryLength);
  }

  return ret;
}

size_t AFile::_peekNetworkOrder(u1* pData, size_t uLength)
{
  size_t ret = 0x0;

  switch(uLength)
  {
    case 0x1:
      ret = peek(pData, 0x1);
    break;

    case 0x2:
      ret = _peekNetworkOrderU2(pData);
    break;

    case 0x4:
      ret = _peekNetworkOrdersize_t(pData);
    break;

    case 0x8:
      ret = _peekNetworkOrderU8(pData);
    break;

    default:
      ATHROW(this, AException::InvalidBinaryLength);
  }

  return ret;
}

size_t AFile::_writeNetworkOrderU2(const u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x2];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x3];
  pBuffer[0x1] = pData[0x2];
  
  //a_Write 2 bytes
  ret = write(pBuffer, 0x2);
#else
  ret = write(pData, 0x2);
#endif

  return ret;
}

size_t AFile::_readNetworkOrderU2(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x2];
  
  //a_Read 2 bytes
  ret = read(pBuffer, 0x2);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x1];
  pData[0x1] = pBuffer[0x0];
#else
  ret = read(pData, 0x2);
#endif

  return ret;
}

size_t AFile::_peekNetworkOrderU2(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x2];
  
  //a_Read 2 bytes
  ret = peek(pBuffer, 0x2);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x1];
  pData[0x1] = pBuffer[0x0];
#else
  ret = peek(pData, 0x2);
#endif

  return ret;
}

size_t AFile::_writeNetworkOrdersize_t(const u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x4];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x3];
  pBuffer[0x1] = pData[0x2];
  pBuffer[0x2] = pData[0x1];
  pBuffer[0x3] = pData[0x0];
  
  //a_Write 4 bytes
  ret = write(pBuffer, 0x4);
#else
  ret = write(pData, 0x4);
#endif

  return ret;
}

size_t AFile::_readNetworkOrdersize_t(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x4];
  
  //a_Read 4 bytes
  ret = read(pBuffer, 0x4);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x3];
  pData[0x1] = pBuffer[0x2];
  pData[0x2] = pBuffer[0x1];
  pData[0x3] = pBuffer[0x0];
#else
  ret = read(pData, 0x4);
#endif

  return ret;
}

size_t AFile::_peekNetworkOrdersize_t(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x4];
  
  //a_Read 4 bytes
  ret = peek(pBuffer, 0x4);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x3];
  pData[0x1] = pBuffer[0x2];
  pData[0x2] = pBuffer[0x1];
  pData[0x3] = pBuffer[0x0];
#else
  ret = peek(pData, 0x4);
#endif

  return ret;
}

size_t AFile::_writeNetworkOrderU8(const u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x8];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x7];
  pBuffer[0x1] = pData[0x6];
  pBuffer[0x2] = pData[0x5];
  pBuffer[0x3] = pData[0x4];
  pBuffer[0x4] = pData[0x3];
  pBuffer[0x5] = pData[0x2];
  pBuffer[0x6] = pData[0x1];
  pBuffer[0x7] = pData[0x0];
  
  //a_Write 8 bytes
  ret = write(pBuffer, 0x8);
#else
  ret = write(pData, 0x8);
#endif

  return ret;
}

size_t AFile::_readNetworkOrderU8(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x8];
  
  //a_Read 8 bytes
  ret = read(pBuffer, 0x8);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x7];
  pData[0x1] = pBuffer[0x6];
  pData[0x2] = pBuffer[0x5];
  pData[0x3] = pBuffer[0x4];
  pData[0x4] = pBuffer[0x3];
  pData[0x5] = pBuffer[0x2];
  pData[0x6] = pBuffer[0x1];
  pData[0x7] = pBuffer[0x0];
#else
  ret = read(pData, 0x8);
#endif

  return ret;
}

size_t AFile::_peekNetworkOrderU8(u1* pData)
{
  size_t ret = 0x0;

#ifdef _WIN32_
  u1 pBuffer[0x8];
  
  //a_Read 8 bytes
  ret = peek(pBuffer, 0x8);
  
  //a_Reverse
  pData[0x0] = pBuffer[0x7];
  pData[0x1] = pBuffer[0x6];
  pData[0x2] = pBuffer[0x5];
  pData[0x3] = pBuffer[0x4];
  pData[0x4] = pBuffer[0x3];
  pData[0x5] = pBuffer[0x2];
  pData[0x6] = pBuffer[0x1];
  pData[0x7] = pBuffer[0x0];
#else
  ret = peek(pData, 0x8);
#endif

  return ret;
}

void AFile::putBack(char c)
{
  m_LookaheadBuffer.pushFront(&c, 1);
}

void AFile::putBack(const AEmittable& source)
{
  ARopeDeque rd;
  source.emit(rd);
  m_LookaheadBuffer.pushFront(rd);
}

void AFile::_append(const char *pcc, size_t length)
{
  if (AConstant::npos == length)
    length = (size_t)strlen(pcc);

  write(pcc, length);
}

void AFile::emit(AOutputBuffer& target) const
{
  //a_Cast away the constness, because the object uses internal buffer to maintain readahead
  //a_Peeking fills it up and in this case constness does not apply
  AFile *pFile = const_cast<AFile *>(this);
  pFile->peekUntilEOF(target);
}
