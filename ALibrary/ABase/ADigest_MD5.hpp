/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED_ADigest_MD5_HPP_
#define INCLUDED_ADigest_MD5_HPP_

#include "apiABase.hpp"
#include "ADigestInterface.hpp"

class AFile;

class ABASE_API ADigest_MD5 : public ADigestInterface
{
public:
  ADigest_MD5();
  virtual ~ADigest_MD5() {}

  /*!
   Usage:
   1. call update as many times as needed
   2. call finalize
   3. use getDigest() for binary digest or emit() for ASCII
   4. call reset and start again
  **/
  virtual void reset();
  virtual void update(AFile&);
  virtual void update(const AString&, size_t inputLen = AConstant::npos);  //a_inputLen can be specified for partial buffers
  virtual void finalize();

  /*!
  Get result AFTER finalize() was called
  */
  virtual const AString& getDigest() const;     //a_Binary form
  virtual void emit(AOutputBuffer&) const;             //a_ASCII emit using ATextConverter::encodeHEX

private:
  //RSA MD5 implementation
  void _transform(const AString& strData, u4 offset);
  inline u4 _rotate_left(u4 x, int n);
  inline void _FF( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T);
  inline void _GG( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T);
  inline void _HH( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T);
  inline void _II( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T);

  //utility functions
  inline void _u4_to_u1(AString& Output, const u4 *Input, u4 nLength);
  inline void _u1_to_u4(u4 *Output, const char *Input, u4 nLength);

  //a_Padding data
  static const AString sstr_MD5Padding;

  AString mstr_Buffer;    //a_Work buffer
  u4 m_count[2];          //a_number of bits, modulo 2^64 (lsb first)
  u4 m_state[4];          //a_current MD5 checksum state
  AString mstr_Digest;    //a_Result set after finalize is called
  bool mbool_Finalized;   //a_If finalize was already called
};

/////////////////////////////////////////////////////////////////////////
// ADigest_MD5 is an implementation of MD5 algorithm
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
// These notices must be retained in any copies of any part of this
// documentation and/or software.
/////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_ADigest_MD5_HPP_
