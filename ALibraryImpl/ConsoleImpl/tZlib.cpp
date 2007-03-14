#include "AZlib.hpp"
#include "AString.hpp"

#include "include/zlib.h"
#if defined(__WINDOWS__)
#pragma comment(lib, "zlib1")
#endif

void testCompress()
{
  AString str("This is some raw data and This is some raw data and This is some raw data and This is some raw data and on and on and on");
  AString compressed, uncompressed;

  AZlib::deflate(str, compressed, 9);
  AZlib::inflate(compressed, uncompressed, str.getSize());

  std::cout << "comressed.size=" << compressed.getSize() << std::endl;
  std::cout << "uncomressed.size=" << uncompressed.getSize() << std::endl;
  std::cout << "uncomressed=" << uncompressed << std::endl;
}

void testDeflate()
{
/*
typedef struct z_stream_s {
    Bytef    *next_in;  // next input byte //
    uInt     avail_in;  // number of bytes available at next_in //
    uLong    total_in;  // total nb of input bytes read so far //

    Bytef    *next_out; // next output byte should be put there //
    uInt     avail_out; // remaining free space at next_out //
    uLong    total_out; // total nb of bytes output so far //

    char     *msg;      // last error message, NULL if no error //
    struct internal_state FAR *state; // not visible by applications //

    alloc_func zalloc;  // used to allocate the internal state //
    free_func  zfree;   // used to free the internal state //
    voidpf     opaque;  // private data object passed to zalloc and zfree //

    int     data_type;  // best guess about the data type: binary or text //
    uLong   adler;      // adler32 value of the uncompressed data //
    uLong   reserved;   // reserved for future use //
} z_stream;
*/
  
  AString str("This is some raw data and This is some raw data and This is some raw data and This is some raw data and on and on and on");
  AString compressed, uncompressed;

  z_stream zs;
  memset ((void *)&zs, 0, sizeof(z_stream));

  zs.next_in = (Bytef *)str.startUsingCharPtr();
  zs.avail_in = str.getSize();
  
  u4 possibleSize = ::deflateBound(&zs, str.getSize());
  zs.next_out = (Bytef *)compressed.startUsingCharPtr(possibleSize);
  zs.avail_out = possibleSize;

  int level = 6;
  int err = deflateInit(&zs, level);
  if (err == Z_OK)
  {
    err = deflate(&zs, Z_FINISH);
    if (err == Z_STREAM_END) {
      compressed.stopUsingCharPtr(zs.total_out);
    }

    err = deflateEnd(&zs);
  }

  if (Z_OK != err)
  {
    std::cout << "Deflate: Error occured: " << err << std::endl;
    return;
  }

  //a_Uncompress
  memset ((void *)&zs, 0, sizeof(z_stream));

  zs.next_in = (Bytef*)compressed.startUsingCharPtr();
  zs.avail_in = compressed.getSize();
  zs.next_out = (Bytef *)uncompressed.startUsingCharPtr(str.getSize());
  zs.avail_out = str.getSize();

  err = inflateInit(&zs);
  if (Z_OK == err)
  {
    err = inflate(&zs, Z_FINISH);
    if (err != Z_STREAM_END) {
      if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && zs.avail_in == 0))
      {
        std::cout << "Inflate: Data error occured: " << err << std::endl;
        return; //Z_DATA_ERROR;
      }
    }

    err = inflateEnd(&zs);
    uncompressed.stopUsingCharPtr(zs.total_out);
  }

  if (Z_OK != err)
  {
    std::cout << "Inflate: Error occured: " << err << std::endl;
    return;
  }

  std::cout << "O: " << str << std::endl;
  std::cout << "R: " << uncompressed << std::endl;
}

int main()
{

//  testCompress();
  testDeflate();

  return 0;
}
