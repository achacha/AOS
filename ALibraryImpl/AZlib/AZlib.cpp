#include "pchAZlib.hpp"
#include "AZlib.hpp"
#include "AString.hpp"
#include "AException.hpp"

#if defined(__WINDOWS__)
#pragma comment(lib, "zlib1")
#endif

void AZlib::deflate(const AString& source, AString& target, int level)
{
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  uLongf newSize = ::compressBound(source.getSize());
  int ret = ::compress2(
    (Bytef *)target.startUsingCharPtr(newSize),   // target buffer
    &newSize,                                     // new size of target buffer
    (const Bytef *)source.c_str(),                // source buffer
    source.getSize(),                             // source size
    level                                         // compression level
  );

  switch(ret)
  {
    case Z_MEM_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    case Z_BUF_ERROR:
      ATHROW(NULL, AException::ProgrammingError);

    case Z_OK:
      target.stopUsingCharPtr(newSize);
  }
}

void AZlib::inflate(const AString& source, AString& target, size_t uncompressedSize)
{
  AASSERT(NULL, uncompressedSize < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  AASSERT(NULL, source.getSize() < ULONG_MAX);          //a_Zlib supports up to 32 bits for now
  uLongf newSize = (uLongf)uncompressedSize;
  int ret = ::uncompress(
    (Bytef *)target.startUsingCharPtr(newSize),         // target buffer
    &newSize,                                           // new size of target buffer
    (const Bytef *)source.c_str(),                      // source buffer
    source.getSize()                                    // source size
  );

  switch(ret)
  {
    case Z_BUF_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_DATA_ERROR :
      ATHROW(NULL, AException::InvalidData);

    case Z_MEM_ERROR :
      ATHROW(NULL, AException::ProgrammingError);

    case Z_OK:
      target.stopUsingCharPtr(newSize);
  }
}
