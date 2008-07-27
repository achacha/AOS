/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAZlib.hpp"
#include "AZlib.hpp"
#include "AString.hpp"
#include "AException.hpp"

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif

//a_Windows OS assumed, not important for us
#define OS_CODE  0x00

void AZlib::zlibDeflate(const AString& source, AString& target, int level)
{
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  int ret = _zlibCompress(source, target, level, true);

  switch(ret)
  {
    case Z_MEM_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    case Z_BUF_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    default:
    case Z_OK:
    break;
  }
}

void AZlib::zlibInflate(const AString& source, AString& target, size_t uncompressedSize)
{
  AASSERT(NULL, uncompressedSize < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  int ret = _zlibUncompress(source, target, uncompressedSize, true);

  switch(ret)
  {
    case Z_BUF_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_DATA_ERROR :
      ATHROW(NULL, AException::InvalidData);

    case Z_MEM_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_OK:
    default:
    break;
  }
}

void AZlib::gzipDeflate(const AString& source, AString& target, int level)
{
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  int ret = _zlibCompress(source, target, level, false);

  switch(ret)
  {
    case Z_MEM_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    case Z_BUF_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    default:
    case Z_OK:
    break;
  }
}

void AZlib::gzipInflate(const AString& source, AString& target, size_t uncompressedSize)
{
  AASSERT(NULL, uncompressedSize < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  int ret = _zlibUncompress(source, target, uncompressedSize, false);

  switch(ret)
  {
    case Z_BUF_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_DATA_ERROR :
      ATHROW(NULL, AException::InvalidData);

    case Z_MEM_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_OK:
    default:
    break;
  }
}

int AZlib::_zlibCompress(const AString& source, AString &target, int level, bool useZlib)
{
  z_stream stream;
  int err;

  uLongf newSize = ::compressBound(source.getSize());

  int destOffset = 0;
  if (!useZlib)
  {
    //a_Create simple gzip header
    target.append("\x1f\x8b",2);
    target.append((char)Z_DEFLATED);
    target.append("\x00\x00\x00\x00\x00\x00",6);
    target.append((char)OS_CODE);
    destOffset = 10;
  }

  stream.next_in = (Bytef*)(const Bytef *)source.c_str();
  stream.avail_in = (uInt)source.getSize();
#ifdef MAXSEG_64K
  /* Check for source > 64K on 16-bit machine: */
  if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
  stream.next_out = (Bytef *)(target.startUsingCharPtr(newSize+destOffset)+destOffset);    //a_Add 10 bytes for the possible simple gzip header
  stream.avail_out = (uInt)newSize;
  if ((uLong)stream.avail_out != newSize) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;

  if (useZlib)
    err = deflateInit(&stream, level);
  else
    err = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

  if (err != Z_OK) return err;

  err = deflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) {
      deflateEnd(&stream);
      return err == Z_OK ? Z_BUF_ERROR : err;
  }
  target.stopUsingCharPtr(stream.total_out+destOffset);

  err = deflateEnd(&stream);
  return err;
}

int AZlib::_zlibUncompress(const AString& source, AString& target, size_t uncompressedSize, bool useZlib)
{
  z_stream stream;
  int err;

  uLongf newSize = (uLongf)uncompressedSize;
  int sourceOffset = 0;
  if (!useZlib)
  {
    //a_Skip over the gzip header
    if (0x1f == source.at(0) && 0x8b == source.at(1) && Z_DEFLATED == source.at(2))
      sourceOffset += 10;
    else
      ATHROW(&source, AException::InvalidData);
  }

  stream.next_in = (Bytef*)(const Bytef *)(source.c_str() + sourceOffset);
  stream.avail_in = (uInt)(source.getSize()-sourceOffset);
  /* Check for source > 64K on 16-bit machine: */
  if ((uLong)stream.avail_in != (uInt)(source.getSize()-sourceOffset)) return Z_BUF_ERROR;

  stream.next_out = (Bytef *)target.startUsingCharPtr(newSize);
  stream.avail_out = newSize;
  if ((uLong)stream.avail_out != newSize) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;

  if (useZlib)
    err = inflateInit(&stream);
  else
    err = inflateInit2(&stream, -MAX_WBITS);

  if (err != Z_OK) return err;

  err = inflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) {
      inflateEnd(&stream);
      if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
          return Z_DATA_ERROR;
      return err;
  }
  target.stopUsingCharPtr(stream.total_out);

  err = inflateEnd(&stream);
  return err;
}
