#include "pchABase.hpp"
#include "AOutputBuffer.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AException.hpp"

size_t AOutputBuffer::append(const char *pcc)
{
  AASSERT(this, pcc);
  size_t len = strlen(pcc);
  return _append(pcc, len);
}

size_t AOutputBuffer::append(const char *pcc, size_t len)
{
  AASSERT(this, pcc);
  if (!len)
    return 0;

  return _append(pcc, len);
}

size_t AOutputBuffer::append(const void *pcc, size_t len)
{
  AASSERT(this, pcc);
  if (!len)
    return 0;

  return _append((const char *)pcc, len);
}

void AOutputBuffer::append(const AEmittable& source)
{
  source.emit(*this);
}

size_t AOutputBuffer::append(const AString& str, size_t len)
{
  if (!len)
    return 0;

  if (AConstant::npos == len)
    return _append(str.c_str(), str.getSize());
  else
  {
    AASSERT(this, len <= str.getSize());
    return _append(str.c_str(), len);
  }
}

size_t AOutputBuffer::append(char d)
{
  return _append((const char *)&d, 1);
}

size_t AOutputBuffer::appendInt(int d)
{
#ifdef _WIN64
  return appendU8(u8(d));
#else
  return appendU4(u4(d));
#endif
}

size_t AOutputBuffer::appendInt_LSB(int d)
{
#ifdef _WIN64
  return appendU8_LSB(u8(d));
#else
  return appendU4_LSB(u4(d));
#endif
}

size_t AOutputBuffer::appendU1(u1 d)
{
  return _append((const char *)&d, 1);
}

size_t AOutputBuffer::appendU2(u2 d)
{
#ifdef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x2];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x1];
  pBuffer[0x1] = pData[0x0];
  
  //a_Write bytes
  return _append(pBuffer, 0x2);
#else
  return _append((const char *)&d, 0x2);
#endif
}

size_t AOutputBuffer::appendU4(u4 d)
{
#ifdef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x4];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x3];
  pBuffer[0x1] = pData[0x2];
  pBuffer[0x2] = pData[0x1];
  pBuffer[0x3] = pData[0x0];
  
  //a_Write bytes
  return _append(pBuffer, 0x4);
#else
  return _append((const char *)&d, 0x4);
#endif
}

size_t AOutputBuffer::appendU8(u8 d)
{
#ifdef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x8];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x7];
  pBuffer[0x1] = pData[0x6];
  pBuffer[0x2] = pData[0x5];
  pBuffer[0x3] = pData[0x4];
  pBuffer[0x4] = pData[0x3];
  pBuffer[0x5] = pData[0x2];
  pBuffer[0x6] = pData[0x1];
  pBuffer[0x7] = pData[0x0];
  
  //a_Write bytes
  return _append(pBuffer, 0x8);
#else
  return _append((const char *)&d, 0x8);
#endif
}

size_t AOutputBuffer::appendU2_LSB(u2 d)
{
#ifndef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x2];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x1];
  pBuffer[0x1] = pData[0x0];
  
  //a_Write bytes
  return _append(pBuffer, 0x2);
#else
  return _append((const char *)&d, 0x2);
#endif
}

size_t AOutputBuffer::appendU4_LSB(u4 d)
{
#ifndef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x4];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x3];
  pBuffer[0x1] = pData[0x2];
  pBuffer[0x2] = pData[0x1];
  pBuffer[0x3] = pData[0x0];
  
  //a_Write 4 bytes
  return _append(pBuffer, 0x4);
#else
  return _append((const char *)&d, 0x4);
#endif
}

size_t AOutputBuffer::appendU8_LSB(u8 d)
{
#ifndef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x8];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x7];
  pBuffer[0x1] = pData[0x6];
  pBuffer[0x2] = pData[0x5];
  pBuffer[0x3] = pData[0x4];
  pBuffer[0x4] = pData[0x3];
  pBuffer[0x5] = pData[0x2];
  pBuffer[0x6] = pData[0x1];
  pBuffer[0x7] = pData[0x0];
  
  //a_Write 4 bytes
  return _append(pBuffer, 0x8);
#else
  return _append((const char *)&d, 0x8);
#endif
}

void AOutputBuffer::appendLine(const AEmittable& source)
{
  source.emit(*this);
  AConstant::ASTRING_EOL.emit(*this);
}
