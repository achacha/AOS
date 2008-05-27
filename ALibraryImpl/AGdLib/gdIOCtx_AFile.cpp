/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAGdLib.hpp"
#include "gdIOCtx_AFile.hpp"
#include "AException.hpp"
#include "AFile_Physical.hpp"

gdIOCtx_AFile::gdIOCtx_AFile(AFile& file) :
  m_File(file)
{
  m_IOCtx.getC = gdIOCtx_AFile::getC;
  m_IOCtx.getBuf = gdIOCtx_AFile::getBuf;
  m_IOCtx.putC = gdIOCtx_AFile::putC;
  m_IOCtx.putBuf = gdIOCtx_AFile::putBuf;
  m_IOCtx.seek = gdIOCtx_AFile::seek;
  m_IOCtx.tell = gdIOCtx_AFile::tell;
  m_IOCtx.gd_free = gdIOCtx_AFile::gd_free;
}

int gdIOCtx_AFile::getC(struct gdIOCtx *pThisBase)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;
  
  u1 data;
  pThis->m_File.read(data);
  return data;
}

int gdIOCtx_AFile::getBuf(struct gdIOCtx *pThisBase, void *pData, int length)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;

  return pThis->m_File.read(pData, length);
}

void gdIOCtx_AFile::putC(struct gdIOCtx *pThisBase, int data)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;

  pThis->m_File.write((u1)data);
}

int gdIOCtx_AFile::putBuf(struct gdIOCtx *pThisBase, const void *pData, int length)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;

  return pThis->m_File.write(pData, length);
}

int gdIOCtx_AFile::seek(struct gdIOCtx *pThisBase, const int pos)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;

  AFile_Physical *pFile = dynamic_cast<AFile_Physical *>(&(pThis->m_File));
  if (!pFile)
    ATHROW_EX(NULL, AException::TypeidMismatch, ASWNL("AFile object does not support seek operation"));

  return (pFile->seek(pos) ? 1 : 0);
}

long gdIOCtx_AFile::tell(struct gdIOCtx *pThisBase)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;

  AFile_Physical *pFile = dynamic_cast<AFile_Physical *>(&(pThis->m_File));
  if (!pFile)
    ATHROW_EX(NULL, AException::TypeidMismatch, ASWNL("AFile object does not support seek operation"));

  return pFile->tell();
}

void gdIOCtx_AFile::gd_free(struct gdIOCtx *pThisBase)
{
  gdIOCtx_AFile *pThis = (gdIOCtx_AFile *)pThisBase;
}
