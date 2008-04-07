#include "pchABase.hpp"
#pragma hdrstop

#include "AFile.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "AThread.hpp"

void AFile::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReadBlock=" << m_ReadBlock << std::endl;
  m_LookaheadBuffer.debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

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
  if (AConstant::npos == ret && !_isNotEof())
  {
    if (treatEofAsEol)
    {
      ret = m_LookaheadBuffer.getSize();
      if (!ret)
        return AConstant::npos;       //a_No more data

      m_LookaheadBuffer.emit(strRead);
      m_LookaheadBuffer.clear();
      return ret;
    }
    else
      return AConstant::npos;
  }

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      case 0:
      case AConstant::npos:
        if (treatEofAsEol)
        {
          //a_Read to end and return
          ret = m_LookaheadBuffer.getSize();
          if (!ret)
            return AConstant::npos;       //a_No more data

          m_LookaheadBuffer.emit(strRead);
          m_LookaheadBuffer.clear();
          return ret;
        }
        else
          return AConstant::npos;       //a_No end of line found

      default:
        ret = m_LookaheadBuffer.popFrontUntil(strRead, '\n', true, true, maxBytes);
    }
  }

  if (
       ret > 0 
    && strRead.getSize() > originalSize 
    && strRead.last() == '\r'
  )
  {
    //a_Remove CR that was before LF
    strRead.rremove();
  }
  
  //a_Check if blank line read
  if (originalSize == strRead.getSize())
  {
    return 0;
  }

  if (originalSize == strRead.getSize() && !isNotEof() && m_LookaheadBuffer.isEmpty())
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
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case 0:
        return AConstant::npos;     //No more data to read then not found
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return  AConstant::unavail;
      continue;

      case AConstant::npos:
        if (treatEofAsEol)
          ret = m_LookaheadBuffer.peekFront(strPeek, maxBytes);
        else
          ret = AConstant::npos;

      default:
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
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case 0:
        return AConstant::npos;     //No more data to read then not found
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      case AConstant::npos:
        if (treatEofAsEol)
          ret = m_LookaheadBuffer.removeFront(maxBytes);
        else
          ret = AConstant::npos;

      default:
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
  size_t ret = m_LookaheadBuffer.popFrontUntilOneOf(strRead, strDelimeters, boolRemoveDelimiter, boolDiscardDelimiter);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.popFrontUntilOneOf(strRead, strDelimeters, boolRemoveDelimiter, boolDiscardDelimiter);
    }
  }

  return ret;
}

size_t AFile::peekUntilOneOf(AString &strPeek, const AString &strDelimeters)
{
  size_t ret = m_LookaheadBuffer.peekFrontUntilOneOf(strPeek, strDelimeters);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.peekFrontUntilOneOf(strPeek, strDelimeters);
    }
  }
  return ret;
}

size_t AFile::skipUntilOneOf(
  const AString &strDelimeters, 
  bool boolDiscardDelimeter // = true
)
{
  size_t ret = m_LookaheadBuffer.removeFrontUntilOneOf(strDelimeters, boolDiscardDelimeter);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.removeFrontUntilOneOf(strDelimeters, boolDiscardDelimeter);
    }
  }

  return ret;
}

size_t AFile::skipUntilOneOf(
  char delimeter, 
  bool boolDiscardDelimeter // = true
)
{
  AString strDelim(delimeter);
  return skipUntilOneOf(strDelim, boolDiscardDelimeter);
}

size_t AFile::peekUntil(AString& strPeek, const AString& strPattern)
{
  size_t ret = m_LookaheadBuffer.peekFrontUntil(strPeek, strPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.peekFrontUntil(strPeek, strPattern);
    }
  }
  return ret;
}

size_t AFile::peekUntil(AString& strPeek, char cPattern)
{
  size_t ret = m_LookaheadBuffer.peekFrontUntil(strPeek, cPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.peekFrontUntil(strPeek, cPattern);
    }
  }
  return ret;
}

size_t AFile::readUntil(
  AString& strRead, 
  const AString& strPattern, 
  bool boolRemovePattern,   // = true 
  bool boolDiscardPattern   // = true
)
{
  size_t ret = m_LookaheadBuffer.popFrontUntil(strRead, strPattern, boolRemovePattern, boolDiscardPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.popFrontUntil(strRead, strPattern, boolRemovePattern, boolDiscardPattern);
    }
  }

  return ret;
}

size_t AFile::readUntil(
  AString& strRead, 
  char cPattern, 
  bool boolRemovePattern,   // = true 
  bool boolDiscardPattern   // = true
)
{
  size_t ret = m_LookaheadBuffer.popFrontUntil(strRead, cPattern, boolRemovePattern, boolDiscardPattern);

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.popFrontUntil(strRead, cPattern, boolRemovePattern, boolDiscardPattern);
    }
  }

  return ret;
}

size_t AFile::skipUntil(
  const AString& strPattern, 
  bool boolDiscardPattern // = true
)
{
  size_t ret = m_LookaheadBuffer.removeFrontUntil(strPattern, boolDiscardPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.removeFrontUntil(strPattern, boolDiscardPattern);
    }
  }

  return ret;
}

size_t AFile::skipUntil(
  char cPattern, 
  bool boolDiscardPattern // = true
)
{
  size_t ret = m_LookaheadBuffer.removeFrontUntil(cPattern, boolDiscardPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.removeFrontUntil(cPattern, boolDiscardPattern);
    }
  }

  return ret;
}

size_t AFile::skipOver(
  const AString& strDelimeters  // = AConstant::ASTRING_WHITESPACE
)
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
  size_t ret = m_LookaheadBuffer.find(strPattern);
  if (AConstant::npos == ret && !_isNotEof())
    return AConstant::npos;

  while ((AConstant::npos == ret || AConstant::unavail == ret) && _isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch(ret)
    {
      case AConstant::npos:
      case 0:
        return AConstant::npos;
      
      case AConstant::unavail:
        //a_No data available yet, wait and try again
        return AConstant::unavail;
      continue;

      default:
        ret = m_LookaheadBuffer.find(strPattern);
    }
  }
  return ret;
}

size_t AFile::writeLine(
  const AString& line,    // = AConstant::ASTRING_EMPTY
  const AString& strEOL   // = AConstant::ASTRING_EOL
)
{
  size_t written = 0;
  if (line.getSize() > 0)
  {
    written = _write(line.data(), line.getSize());
  }

  if (AConstant::npos != written && AConstant::unavail != written)
    written += _write(strEOL.data(), strEOL.getSize());
  
  return written;
}

size_t AFile::write(const void *pcvBuffer, size_t length)
{
  return _write(pcvBuffer, length);
}

size_t AFile::read(
  AOutputBuffer& target, 
  size_t length
)
{
  const int BUFFER_SIZE = 4096;
  AString buffer(BUFFER_SIZE, 256);
  size_t bytesRead = 0;
  size_t totalBytesRead = 0;
  while (length > 0)
  {
    size_t bytesToRead = min(BUFFER_SIZE, length);
    bytesRead = read(buffer, bytesToRead);
    
    target.append(buffer, bytesRead);

    length -= bytesRead;
    totalBytesRead += bytesRead;
    
    //a_Check for partial read which usually signifies EOF
    if (bytesRead != bytesToRead)
      break;
  }
  
  return totalBytesRead;
}

size_t AFile::write(
  AFile& source, 
  size_t length // = AConstant::npos
)
{
  return source.read(*this, length);
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

size_t AFile::write(
  const AString& line, 
  size_t length // = AConstant::npos
)
{
  AASSERT(this, length <= line.getSize());
  if (length != AConstant::npos)
    return _write(line.data(), length);
  else
    return _write(line.data(), line.getSize());
}

size_t AFile::read(ARope& rope, size_t length)
{
  return rope.read(*this, length);
}

size_t AFile::write(const ARope& rope)
{
  return rope.write(*this);
}

size_t AFile::peekUntilEOF(AOutputBuffer& target)
{
  //a_Read everything into lookahead buffer
  size_t ret = readBlockIntoLookahead();
  while (_isNotEof())
  {
    ret = readBlockIntoLookahead();
    switch (ret)
    {
      case AConstant::unavail:
        return AConstant::unavail;
      continue;

      case AConstant::npos:
        return m_LookaheadBuffer.getSize();  // Done
    }
  }

  m_LookaheadBuffer.emit(target);

  return m_LookaheadBuffer.getSize();
}

size_t AFile::readUntilEOF(AOutputBuffer& target)
{
  //a_Read everything
  size_t bytesRead = 0;
  while (_isNotEof())
  {
    switch (readBlockIntoLookahead())
    {
      case AConstant::unavail:
        return AConstant::unavail;
      continue;

      case AConstant::npos:
        break;

      default:
        bytesRead += m_LookaheadBuffer.getSize();
        m_LookaheadBuffer.emit(target);
        m_LookaheadBuffer.clear();
    }
  }

  if (!m_LookaheadBuffer.isEmpty())
  {
    bytesRead += m_LookaheadBuffer.getSize();
    m_LookaheadBuffer.emit(target);
    m_LookaheadBuffer.clear();
  }

  return bytesRead;
}

size_t AFile::readBlockIntoLookahead()
{
  AString str;
  char *p = str.startUsingCharPtr(m_ReadBlock);
  
  size_t bytesRead = _read(p, m_ReadBlock);
  
  switch(bytesRead)
  {
    case AConstant::npos:
    case 0:
      return AConstant::npos;

    case AConstant::unavail:
      return AConstant::unavail;

    default:
      m_LookaheadBuffer.pushBack(p, bytesRead);
  }

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
    size_t bytesReadPass = readBlockIntoLookahead();
    
    if (AConstant::unavail == bytesReadPass)
      return AConstant::unavail;

    // 0 or AConstant::npos means we are done with reading, handle what we have so far
    if (AConstant::npos == bytesReadPass || !bytesReadPass)
      break;

    bytesRead += bytesReadPass;
  }

  if (bytesRead > bytesLeft)
    bytesRead = bytesLeft;

  AASSERT(this, AConstant::npos != bytesRead);
  AASSERT(this, AConstant::unavail != bytesRead);
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
    if (!_isNotEof())
      return 0;
    
    //a_Read file memory until we have what we need
    while (m_LookaheadBuffer.getSize() < bytesLeft && _isNotEof())
    {
      size_t ret = readBlockIntoLookahead();
      switch(ret)
      {
        case AConstant::unavail:
          return AConstant::unavail;
        continue;

        case AConstant::npos:
        case 0:
        {
          //a_Not enough data, return what we have
          ret = m_LookaheadBuffer.getSize();
          if (ret > 0)
            m_LookaheadBuffer.peekFront((char *)pTarget, ret);
          
          return ret;
        }
      }
    }

    m_LookaheadBuffer.peekFront((char *)pTarget, bytesLeft);
    return bytesLeft;
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

size_t AFile::_append(const char *pcc, size_t length)
{
  if (AConstant::npos == length)
    length = (size_t)strlen(pcc);

  return write(pcc, length);
}

void AFile::emit(AOutputBuffer& target) const
{
  //a_Cast away the constness, because the object uses internal buffer to maintain readahead
  AFile *pFile = const_cast<AFile *>(this);
  pFile->readUntilEOF(target);
}

size_t AFile::getSize() const
{
  return AConstant::npos;
}
