#ifndef INCLUDED_ADigest_SHA1_HPP_
#define INCLUDED_ADigest_SHA1_HPP_

#include "apiABase.hpp"
#include "ADigestInterface.hpp"

class AFile;

#define SHA1_DIGEST_LENGTH 20

class ABASE_API ADigest_SHA1 : public ADigestInterface
{
public:
  ADigest_SHA1();
  virtual ~ADigest_SHA1() {}

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
  struct SHA1Context
  {
    u4 state[5];
    u4 count[2];
    unsigned char buffer[64];
    unsigned char digest[SHA1_DIGEST_LENGTH];
  } m_Context;

  //SHA-1 implementation
  void _transform(u4 state[5], const u1 buffer[64]);
  void _update(const u1* data, size_t len);
  
  AString mstr_Digest;    //a_Result set after finalize is called
  bool mbool_Finalized;   //a_If finalize was already called
};

/*
Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#endif //INCLUDED_ADigest_SHA1_HPP_
