/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__uTypes_HPP__
#define INCLUDED__uTypes_HPP__

#include "osDefines.hpp"
#include <vector>

/*!
  Defines to be used throughout to guarantee byte width
*/
typedef unsigned char     u1;
typedef unsigned short    u2;
typedef unsigned long     u4;
typedef signed   char     s1;
typedef signed   short    s2;
typedef signed   long     s4;

//a_Platform specific
#if defined(__WINDOWS__)

  typedef unsigned __int64 u8;
  typedef signed   __int64 s8;

#elif defined(__UNIX__)

  typedef unsigned long long u8;
  typedef signed   long long s8;

#endif

/*!
Containers
*/
typedef std::vector<u1> VECTOR_u1;
typedef std::vector<u2> VECTOR_u2;
typedef std::vector<u4> VECTOR_u4;
typedef std::vector<u8> VECTOR_u8;
typedef std::vector<size_t> VECTOR_size_t;

/*!
  Convenience macros
  u4 set here by definition is [ byte_3 | byte_2 | byte_1 | byte_0 ] (little-endian 32 bit)
  byte_0 is LSB, byte_3 is MSB
*/
#define SET_U4_0(dw, b) (dw = (dw & 0xffffff00) | u4(b))
#define SET_U4_1(dw, b) (dw = (dw & 0xffff00ff) | (u4(b) << 8));
#define SET_U4_2(dw, b) (dw = (dw & 0xff00ffff) | (u4(b) << 16));
#define SET_U4_3(dw, b) (dw = (dw & 0x00ffffff) | (u4(b) << 24));
#define MAKE_U4(b3, b2, b1, b0) ((b3 << 24) | (b2 << 16) | (b1 << 8) | b0)
#define GET_U4_0(dw) (((u1 *)&dw)[3])
#define GET_U4_1(dw) (((u1 *)&dw)[2])
#define GET_U4_2(dw) (((u1 *)&dw)[1])
#define GET_U4_3(dw) (((u1 *)&dw)[0])

#ifndef errno_t
  typedef int errno_t;
#endif

#endif //INCLUDED__uTypes_HPP__
