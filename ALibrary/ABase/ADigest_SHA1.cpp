/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ADigest_SHA1.hpp"
#include "AException.hpp"
#include "ATextConverter.hpp"
#include "AFile.hpp"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
#ifdef BIG_ENDIAN
  #define blk0(i) block->l[i]
#else
  #define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00)|(rol(block->l[i],8)&0x00FF00FF))
#endif

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15]^block->l[(i+2)&15]^block->l[i&15],1))


/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

ADigest_SHA1::ADigest_SHA1()
{
  reset();
}

void ADigest_SHA1::reset()
{
  /* SHA1 initialization constants */
  m_Context.state[0] = 0x67452301;
  m_Context.state[1] = 0xEFCDAB89;
  m_Context.state[2] = 0x98BADCFE;
  m_Context.state[3] = 0x10325476;
  m_Context.state[4] = 0xC3D2E1F0;
  m_Context.count[0] = m_Context.count[1] = 0;

  mbool_Finalized = false;
}

void ADigest_SHA1::update(AFile& file)
{
  AString strBuffer;
  size_t bytesRead = file.peekUntilEOF(strBuffer);
  update(strBuffer, bytesRead);
}

void ADigest_SHA1::update(const AString& strInput, size_t inputLen /* = AConstant::npos */)
{
  size_t len = (AConstant::npos == inputLen ? strInput.getSize() : inputLen);
  const u1 *data = (const u1 *)strInput.data();
  _update(data, len);
}

void ADigest_SHA1::_update(const u1* data, size_t len)
{
  u4 i, j;

  j = (m_Context.count[0] >> 3) & 63;
  if ((m_Context.count[0] += len << 3) < (len << 3))
    m_Context.count[1]++;

  m_Context.count[1] += (len >> 29);

  if ((j + len) > 63)
  {
    memcpy(&m_Context.buffer[j], data, (i = 64-j));
    _transform(m_Context.state, m_Context.buffer);
    for ( ; i + 63 < len; i += 64)
    {
      _transform(m_Context.state, &data[i]);
    }
    j = 0;
  }
  else
    i = 0;
  memcpy(&m_Context.buffer[j], &data[i], len - i);
}

void ADigest_SHA1::finalize()
{
  if (mbool_Finalized)
    ATHROW(NULL, AException::InvalidState);

  unsigned char finalcount[8];
  u4 i;
  for (i = 0; i < 8; i++) {
    // Endian independent
    finalcount[i] = (u1)((m_Context.count[(i >= 4 ? 0 : 1)] >> ((3-(i & 3)) * 8) ) & 255);
  }
  _update((u1 *)"\x80", 1);
  while ((m_Context.count[0] & 504) != 448)
  {
    _update((u1 *)"\x0", 1);
  }
  _update(finalcount, 8);
  for (i = 0; i < SHA1_DIGEST_LENGTH; i++)
  {
    m_Context.digest[i] = (u1)((m_Context.state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
  }

  // Wipe variables
  i = 0;
  memset(m_Context.buffer, 0, 64);
  memset(m_Context.state, 0, SHA1_DIGEST_LENGTH);
  memset(m_Context.count, 0, 8);
  memset(finalcount, 0, 8);
#ifdef SHA1HANDSOFF  // make SHA1Transform overwrite it's own static vars
  SHA1Transform(m_Context.state, m_Context.buffer);
#endif

  //Finalize
  mstr_Digest.assign(m_Context.digest, SHA1_DIGEST_LENGTH);
  mbool_Finalized = true;
}

void ADigest_SHA1::_transform(u4 state[5], const u1 buffer[64])
{
  u4 a, b, c, d, e;
  typedef union {
    unsigned char c[64];
    u4 l[16];
  } CHAR64LONG16;
  CHAR64LONG16* block;

#ifdef SHA1HANDSOFF
  static unsigned char workspace[64];
  block = (CHAR64LONG16*)workspace;
  memcpy(block, buffer, 64);
#else
  block = (CHAR64LONG16*)buffer;
#endif

  /* Copy m_Context.state[] to working vars */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];

  /* 4 rounds of 20 operations each. Loop unrolled. */
  R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
  R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
  R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
  R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
  R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
  R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
  R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
  R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
  R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
  R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
  R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
  R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
  R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
  R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
  R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
  R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
  R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
  R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
  R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
  R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
  
  /* Add the working vars back into context.state[] */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  /* Wipe variables */
  a = b = c = d = e = 0;
}

const AString& ADigest_SHA1::getDigest() const
{
  if (mbool_Finalized)
    return mstr_Digest;
  else
    ATHROW(NULL, AException::InvalidState);
}

void ADigest_SHA1::emit(AOutputBuffer& target) const
{
  if (mbool_Finalized)
    ATextConverter::encodeHEX(mstr_Digest, target);
  else
    ATHROW(NULL, AException::InvalidState);
}
