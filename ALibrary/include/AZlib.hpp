#ifndef INCLUDED__AZlib_HPP__
#define INCLUDED__AZlib_HPP__

#include "uTypes.hpp"
#include "apiAZlib.hpp"

class AString;

class AZLIB_API AZlib
{
public:
  /*!
  Compress source into target at a given compression level
  level    desc
    0      none
    1      minimal compession, fastest
    6      good middle ground (as per zlib)
    9      best compression, slowest
  */
  static void deflate(const AString& source, AString& target, int level = 6);
  
  /*!
  Uncompress source to target
  uncompressedSize must be known and the the target size after compression
  */
  static void inflate(const AString& source, AString& target, size_t uncompressedSize);
};

#endif //INCLUDED__AZlib_HPP__
