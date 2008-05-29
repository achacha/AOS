/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAGdLib.hpp"
#include "gdIOCtx_AOutputBuffer.hpp"
#include "AException.hpp"
#include "AFile_Physical.hpp"

gdIOCtx_AOutputBuffer::gdIOCtx_AOutputBuffer(AOutputBuffer& buf) :
  m_OutputBuffer(buf)
{
  m_IOCtx.getC = gdIOCtx_AOutputBuffer::getC;
  m_IOCtx.getBuf = gdIOCtx_AOutputBuffer::getBuf;
  m_IOCtx.putC = gdIOCtx_AOutputBuffer::putC;
  m_IOCtx.putBuf = gdIOCtx_AOutputBuffer::putBuf;
  m_IOCtx.seek = gdIOCtx_AOutputBuffer::seek;
  m_IOCtx.tell = gdIOCtx_AOutputBuffer::tell;
  m_IOCtx.gd_free = gdIOCtx_AOutputBuffer::gd_free;
}

int gdIOCtx_AOutputBuffer::getC(struct gdIOCtx *pThisBase)
{
  gdIOCtx_AOutputBuffer *pThis = (gdIOCtx_AOutputBuffer *)pThisBase;
  
  ATHROW(NULL, AException::ProgrammingError);
}

int gdIOCtx_AOutputBuffer::getBuf(struct gdIOCtx *pThisBase, void *pData, int length)
{
  ATHROW(NULL, AException::ProgrammingError);
}

void gdIOCtx_AOutputBuffer::putC(struct gdIOCtx *pThisBase, int data)
{
  gdIOCtx_AOutputBuffer *pThis = (gdIOCtx_AOutputBuffer *)pThisBase;

  pThis->m_OutputBuffer.append((u1)data);
}

int gdIOCtx_AOutputBuffer::putBuf(struct gdIOCtx *pThisBase, const void *pData, int length)
{
  gdIOCtx_AOutputBuffer *pThis = (gdIOCtx_AOutputBuffer *)pThisBase;

  pThis->m_OutputBuffer.append(pData, length);
  return length;
}

int gdIOCtx_AOutputBuffer::seek(struct gdIOCtx *pThisBase, const int pos)
{
  ATHROW(NULL, AException::ProgrammingError);
}

long gdIOCtx_AOutputBuffer::tell(struct gdIOCtx *pThisBase)
{
  ATHROW(NULL, AException::ProgrammingError);
}

void gdIOCtx_AOutputBuffer::gd_free(struct gdIOCtx *pThisBase)
{
}
