/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AZlib_HPP__
#define INCLUDED__AZlib_HPP__

#include "uTypes.hpp"
#include "apiAZlib.hpp"

class AString;

class AZLIB_API AZlib
{
public:
  /*!
  Compress source into target at a given compression level using zlib format
  level    desc
    0      none
    1      minimal compession, fastest
    6      good middle ground (as per zlib)
    9      best compression, slowest
  */
  static void zlibDeflate(const AString& source, AString& target, int level = 6);
  
  /*!
  Uncompress source to target using zlib format
  uncompressedSize must be known and the the target size after compression
  */
  static void zlibInflate(const AString& source, AString& target, size_t uncompressedSize);

  /*!
  Compress source into target at a given compression level using gzip format
  level    desc
    0      none
    1      minimal compession, fastest
    6      good middle ground (as per zlib)
    9      best compression, slowest
  */
  static void gzipDeflate(const AString& source, AString& target, int level = 6);
  
  /*!
  Uncompress source to target using gzip format
  uncompressedSize must be known and the the target size after compression
  */
  static void gzipInflate(const AString& source, AString& target, size_t uncompressedSize);

private:
  //! zlib compress modified for ALibrary
  static int _zlibCompress(const AString& source, AString& target, int level, bool useZlib);

  //! zlib uncompress modified for ALibrary
  static int _zlibUncompress(const AString& source, AString& target, size_t uncompressedSize, bool useZlib);
};

#endif //INCLUDED__AZlib_HPP__
