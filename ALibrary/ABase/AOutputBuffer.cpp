#include "pchABase.hpp"
#include "AOutputBuffer.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AException.hpp"

void AOutputBuffer::append(const char *pcc)
{
  AASSERT(this, pcc);
  size_t len = strlen(pcc);
  _append(pcc, len);
}

void AOutputBuffer::append(const char *pcc, size_t len)
{
  AASSERT(this, pcc);
  if (!len)
    return;

  _append(pcc, len);
}

void AOutputBuffer::append(const void *pcc, size_t len)
{
  AASSERT(this, pcc);
  if (!len)
    return;

  _append((const char *)pcc, len);
}

void AOutputBuffer::append(const AEmittable& source)
{
  source.emit(*this);
}

void AOutputBuffer::append(const AString& str, size_t len)
{
  if (!len)
    return;

  if (AConstant::npos == len)
    _append(str.c_str(), str.getSize());
  else
  {
    AASSERT(this, len <= str.getSize());
    _append(str.c_str(), len);
  }
}

void AOutputBuffer::append(char d)
{
  _append((const char *)&d, 1);
}

void AOutputBuffer::appendInt(int d)
{
#ifdef _WIN64
  appendU8(u8(d));
#else
  appendU4(u4(d));
#endif
}

void AOutputBuffer::appendInt_LSB(int d)
{
#ifdef _WIN64
  appendU8_LSB(u8(d));
#else
  appendU4_LSB(u4(d));
#endif
}

void AOutputBuffer::appendU1(u1 d)
{
  _append((const char *)&d, 1);
}

void AOutputBuffer::appendU2(u2 d)
{
#ifdef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x2];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x1];
  pBuffer[0x1] = pData[0x0];
  
  //a_Write bytes
  _append(pBuffer, 0x2);
#else
  _append((const char *)&d, 0x2);
#endif
}

void AOutputBuffer::appendU4(u4 d)
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
  _append(pBuffer, 0x4);
#else
  _append((const char *)&d, 0x4);
#endif
}

void AOutputBuffer::appendU8(u8 d)
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
  _append(pBuffer, 0x8);
#else
  _append((const char *)&d, 0x8);
#endif
}

void AOutputBuffer::appendU2_LSB(u2 d)
{
#ifndef _WIN32_
  u1 *pData = (u1 *)&d;
  char pBuffer[0x2];
  
  //a_Reverse bytes
  pBuffer[0x0] = pData[0x1];
  pBuffer[0x1] = pData[0x0];
  
  //a_Write bytes
  _append(pBuffer, 0x2);
#else
  _append((const char *)&d, 0x2);
#endif
}

void AOutputBuffer::appendU4_LSB(u4 d)
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
  _append(pBuffer, 0x4);
#else
  _append((const char *)&d, 0x4);
#endif
}

void AOutputBuffer::appendU8_LSB(u8 d)
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
  _append(pBuffer, 0x8);
#else
  _append((const char *)&d, 0x8);
#endif
}

void AOutputBuffer::appendLine(const AEmittable& source)
{
  source.emit(*this);
  AString::sstr_EOL.emit(*this);
}
