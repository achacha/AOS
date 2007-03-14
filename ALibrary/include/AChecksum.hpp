#ifndef INCLUDED__AChecksum_HPP_
#define INCLUDED__AChecksum_HPP_

#include "apiABase.hpp"
#include "AEmittable.hpp"

class ABASE_API AChecksum
{
public:
  static u4 crc32(const AString& data, u4 initialCRC = 0);
  static u4 crc32(u1 data, u4 initialCRC = 0);

  static u4 adler32(const AString& data, u4 initialAdler = 1);
  static u4 adler32(u1 data, u4 initialAdler = 1);

private:
  static u4 crc_table[256];    //a_CRC32 table
};

#endif //INCLUDED__AChecksum_HPP_