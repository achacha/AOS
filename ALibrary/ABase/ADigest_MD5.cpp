#include "pchABase.hpp"
#include "ADigest_MD5.hpp"
#include "AException.hpp"
#include "ATextConverter.hpp"
#include "AFile.hpp"

//a_Padding
const AString ADigest_MD5::sstr_MD5Padding(
"\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 66);

//Magic initialization constants
#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89
#define MD5_INIT_STATE_2 0x98badcfe
#define MD5_INIT_STATE_3 0x10325476

//Constants for _transform routine.
#define MD5_S11  7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21  5
#define MD5_S22  9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31  4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41  6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

//_transformation Constants - Round 1
#define MD5_T01  0xd76aa478 //_transformation Constant 1 
#define MD5_T02  0xe8c7b756 //_transformation Constant 2
#define MD5_T03  0x242070db //_transformation Constant 3
#define MD5_T04  0xc1bdceee //_transformation Constant 4
#define MD5_T05  0xf57c0faf //_transformation Constant 5
#define MD5_T06  0x4787c62a //_transformation Constant 6
#define MD5_T07  0xa8304613 //_transformation Constant 7
#define MD5_T08  0xfd469501 //_transformation Constant 8
#define MD5_T09  0x698098d8 //_transformation Constant 9
#define MD5_T10  0x8b44f7af //_transformation Constant 10
#define MD5_T11  0xffff5bb1 //_transformation Constant 11
#define MD5_T12  0x895cd7be //_transformation Constant 12
#define MD5_T13  0x6b901122 //_transformation Constant 13
#define MD5_T14  0xfd987193 //_transformation Constant 14
#define MD5_T15  0xa679438e //_transformation Constant 15
#define MD5_T16  0x49b40821 //_transformation Constant 16

//_transformation Constants - Round 2
#define MD5_T17  0xf61e2562 //_transformation Constant 17
#define MD5_T18  0xc040b340 //_transformation Constant 18
#define MD5_T19  0x265e5a51 //_transformation Constant 19
#define MD5_T20  0xe9b6c7aa //_transformation Constant 20
#define MD5_T21  0xd62f105d //_transformation Constant 21
#define MD5_T22  0x02441453 //_transformation Constant 22
#define MD5_T23  0xd8a1e681 //_transformation Constant 23
#define MD5_T24  0xe7d3fbc8 //_transformation Constant 24
#define MD5_T25  0x21e1cde6 //_transformation Constant 25
#define MD5_T26  0xc33707d6 //_transformation Constant 26
#define MD5_T27  0xf4d50d87 //_transformation Constant 27
#define MD5_T28  0x455a14ed //_transformation Constant 28
#define MD5_T29  0xa9e3e905 //_transformation Constant 29
#define MD5_T30  0xfcefa3f8 //_transformation Constant 30
#define MD5_T31  0x676f02d9 //_transformation Constant 31
#define MD5_T32  0x8d2a4c8a //_transformation Constant 32

//_transformation Constants - Round 3
#define MD5_T33  0xfffa3942 //_transformation Constant 33
#define MD5_T34  0x8771f681 //_transformation Constant 34
#define MD5_T35  0x6d9d6122 //_transformation Constant 35
#define MD5_T36  0xfde5380c //_transformation Constant 36
#define MD5_T37  0xa4beea44 //_transformation Constant 37
#define MD5_T38  0x4bdecfa9 //_transformation Constant 38
#define MD5_T39  0xf6bb4b60 //_transformation Constant 39
#define MD5_T40  0xbebfbc70 //_transformation Constant 40
#define MD5_T41  0x289b7ec6 //_transformation Constant 41
#define MD5_T42  0xeaa127fa //_transformation Constant 42
#define MD5_T43  0xd4ef3085 //_transformation Constant 43
#define MD5_T44  0x04881d05 //_transformation Constant 44
#define MD5_T45  0xd9d4d039 //_transformation Constant 45
#define MD5_T46  0xe6db99e5 //_transformation Constant 46
#define MD5_T47  0x1fa27cf8 //_transformation Constant 47
#define MD5_T48  0xc4ac5665 //_transformation Constant 48

//_transformation Constants - Round 4
#define MD5_T49  0xf4292244 //_transformation Constant 49
#define MD5_T50  0x432aff97 //_transformation Constant 50
#define MD5_T51  0xab9423a7 //_transformation Constant 51
#define MD5_T52  0xfc93a039 //_transformation Constant 52
#define MD5_T53  0x655b59c3 //_transformation Constant 53
#define MD5_T54  0x8f0ccc92 //_transformation Constant 54
#define MD5_T55  0xffeff47d //_transformation Constant 55
#define MD5_T56  0x85845dd1 //_transformation Constant 56
#define MD5_T57  0x6fa87e4f //_transformation Constant 57
#define MD5_T58  0xfe2ce6e0 //_transformation Constant 58
#define MD5_T59  0xa3014314 //_transformation Constant 59
#define MD5_T60  0x4e0811a1 //_transformation Constant 60
#define MD5_T61  0xf7537e82 //_transformation Constant 61
#define MD5_T62  0xbd3af235 //_transformation Constant 62
#define MD5_T63  0x2ad7d2bb //_transformation Constant 63
#define MD5_T64  0xeb86d391 //_transformation Constant 64

ADigest_MD5::ADigest_MD5()
{
  reset();
}

void ADigest_MD5::reset()
{
  // zero members
//  memset( m_lpszBuffer, 0, 64 );
  mstr_Buffer.clear();
  m_count[0] = m_count[1] = 0;

  // Load magic state initialization constants
  m_state[0] = MD5_INIT_STATE_0;
  m_state[1] = MD5_INIT_STATE_1;
  m_state[2] = MD5_INIT_STATE_2;
  m_state[3] = MD5_INIT_STATE_3;

  mbool_Finalized = false;
}

void ADigest_MD5::update(AFile& file)
{
  AString strBuffer;
  size_t bytesRead = file.peekUntilEOF(strBuffer);
  update(strBuffer, bytesRead);
}

void ADigest_MD5::update(const AString& strInput, size_t inputLen /* = AConstant::npos */)
{
  if (mbool_Finalized)
    ATHROW(NULL, AException::InvalidState);

  if (AConstant::npos == inputLen)
    inputLen = strInput.getSize();

  //Compute number of bytes mod 64
  size_t nIndex = ((m_count[0] >> 3) & 0x3F);

  //Update number of bits
  if ( ( m_count[0] += inputLen << 3 )  <  ( inputLen << 3) )
  {
    m_count[1]++;
  }
  m_count[1] += (inputLen >> 29);

  //_transform as many times as possible.
  size_t i=0;   
  size_t nPartLen = 64 - nIndex;
  if (inputLen >= nPartLen)  
  {
//    memcpy( &m_lpszBuffer[nIndex], strInput.data(), nPartLen );
    mstr_Buffer.set(strInput, nIndex, nPartLen, 0);
    _transform(mstr_Buffer, 0);
    for (i = nPartLen; i + 63 < inputLen; i += 64) 
    {
      _transform(strInput, i);
    }
    nIndex = 0;
  } 
  else 
  {
    i = 0;
  }

  // Buffer remaining input
  mstr_Buffer.set(strInput, nIndex, inputLen-i, i);
}

void ADigest_MD5::finalize()
{
  if (mbool_Finalized)
    ATHROW(NULL, AException::InvalidState);

  //Save number of bits
  AString strBits;
  strBits.setSize(8, '\0');
  _u4_to_u1(strBits, m_count, 8);

  //Pad out to 56 mod 64.
  u4 nIndex = (u4)((m_count[0] >> 3) & 0x3f);
  u4 nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
  update(sstr_MD5Padding, nPadLen);

  //Append length (before padding)
  update(strBits, 8);

  //Store final state in 'lpszMD5'
  const int nMD5Size = 16;
  AString lpszMD5(nMD5Size, nMD5Size);
  _u4_to_u1( lpszMD5, m_state, nMD5Size );

  //Convert the hexadecimal checksum to a AString
  mstr_Digest.assign(lpszMD5, 16);
  mbool_Finalized = true;
}

void ADigest_MD5::_transform(const AString& strData, u4 offset)
{
  //initialise local data with current checksum
  u4 a = m_state[0];
  u4 b = m_state[1];
  u4 c = m_state[2];
  u4 d = m_state[3];

  //copy u1S from input 'Block' to an array of u4S 'X'
  u4 X[16];
  memset(&X, 0, sizeof(u4)*16);
  if (offset + 64 < strData.getSize())
    _u1_to_u4(X, strData.c_str() + offset, 64 );
  else
  {
    AASSERT(NULL, strData.getSize() - offset > 0);
    _u1_to_u4(X, strData.c_str() + offset, strData.getSize() - offset);
  }

  //Perform Round 1 of the transformation
  _FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01); 
  _FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02); 
  _FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03); 
  _FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04); 
  _FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05); 
  _FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06); 
  _FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07); 
  _FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08); 
  _FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09); 
  _FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10); 
  _FF (c, d, a, b, X[10], MD5_S13, MD5_T11); 
  _FF (b, c, d, a, X[11], MD5_S14, MD5_T12); 
  _FF (a, b, c, d, X[12], MD5_S11, MD5_T13); 
  _FF (d, a, b, c, X[13], MD5_S12, MD5_T14); 
  _FF (c, d, a, b, X[14], MD5_S13, MD5_T15); 
  _FF (b, c, d, a, X[15], MD5_S14, MD5_T16); 

  //Perform Round 2 of the transformation
  _GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17); 
  _GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18); 
  _GG (c, d, a, b, X[11], MD5_S23, MD5_T19); 
  _GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20); 
  _GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21); 
  _GG (d, a, b, c, X[10], MD5_S22, MD5_T22); 
  _GG (c, d, a, b, X[15], MD5_S23, MD5_T23); 
  _GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24); 
  _GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25); 
  _GG (d, a, b, c, X[14], MD5_S22, MD5_T26); 
  _GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27); 
  _GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28); 
  _GG (a, b, c, d, X[13], MD5_S21, MD5_T29); 
  _GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30); 
  _GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31); 
  _GG (b, c, d, a, X[12], MD5_S24, MD5_T32); 

  //Perform Round 3 of the transformation
  _HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33); 
  _HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34); 
  _HH (c, d, a, b, X[11], MD5_S33, MD5_T35); 
  _HH (b, c, d, a, X[14], MD5_S34, MD5_T36); 
  _HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37); 
  _HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38); 
  _HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39); 
  _HH (b, c, d, a, X[10], MD5_S34, MD5_T40); 
  _HH (a, b, c, d, X[13], MD5_S31, MD5_T41); 
  _HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42); 
  _HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43); 
  _HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44); 
  _HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45); 
  _HH (d, a, b, c, X[12], MD5_S32, MD5_T46); 
  _HH (c, d, a, b, X[15], MD5_S33, MD5_T47); 
  _HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48); 

  //Perform Round 4 of the transformation
  _II (a, b, c, d, X[ 0], MD5_S41, MD5_T49); 
  _II (d, a, b, c, X[ 7], MD5_S42, MD5_T50); 
  _II (c, d, a, b, X[14], MD5_S43, MD5_T51); 
  _II (b, c, d, a, X[ 5], MD5_S44, MD5_T52); 
  _II (a, b, c, d, X[12], MD5_S41, MD5_T53); 
  _II (d, a, b, c, X[ 3], MD5_S42, MD5_T54); 
  _II (c, d, a, b, X[10], MD5_S43, MD5_T55); 
  _II (b, c, d, a, X[ 1], MD5_S44, MD5_T56); 
  _II (a, b, c, d, X[ 8], MD5_S41, MD5_T57); 
  _II (d, a, b, c, X[15], MD5_S42, MD5_T58); 
  _II (c, d, a, b, X[ 6], MD5_S43, MD5_T59); 
  _II (b, c, d, a, X[13], MD5_S44, MD5_T60); 
  _II (a, b, c, d, X[ 4], MD5_S41, MD5_T61); 
  _II (d, a, b, c, X[11], MD5_S42, MD5_T62); 
  _II (c, d, a, b, X[ 2], MD5_S43, MD5_T63); 
  _II (b, c, d, a, X[ 9], MD5_S44, MD5_T64); 

  //add the transformed values to the current checksum
  m_state[0] += a;
  m_state[1] += b;
  m_state[2] += c;
  m_state[3] += d;
}

u4 ADigest_MD5::_rotate_left(u4 x, int n)
{
  //rotate and return x
  return (x << n) | (x >> (32-n));
}

void ADigest_MD5::_FF( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T)
{
  u4 F = (B & C) | (~B & D);
  A += F + X + T;
  A = _rotate_left(A, S);
  A += B;
}

void ADigest_MD5::_GG( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T)
{
  u4 G = (B & D) | (C & ~D);
  A += G + X + T;
  A = _rotate_left(A, S);
  A += B;
}

void ADigest_MD5::_HH( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T)
{
  u4 H = (B ^ C ^ D);
  A += H + X + T;
  A = _rotate_left(A, S);
  A += B;
}

void ADigest_MD5::_II( u4& A, u4 B, u4 C, u4 D, u4 X, u4 S, u4 T)
{
  u4 I = (C ^ (B | ~D));
  A += I + X + T;
  A = _rotate_left(A, S);
  A += B;
}

void ADigest_MD5::_u4_to_u1(AString& Output, const u4 *Input, u4 nLength)
{
  //entry invariants
  AASSERT(NULL,  nLength % 4 == 0 );
  Output.clear();

  //transfer the data by shifting and copying
  u4 i = 0;
  u4 j = 0;
  for ( ; j < nLength; i++, j += 4) 
  {
    Output.append((char)(Input[i] & 0xff));
    Output.append((char)((Input[i] >> 8) & 0xff));
    Output.append((char)((Input[i] >> 16) & 0xff));
    Output.append((char)((Input[i] >> 24) & 0xff));
  }
}

void ADigest_MD5::_u1_to_u4(u4 *Output, const char *Input, u4 nLength)
{
  //entry invariants
  AASSERT(NULL, nLength % 4 == 0);

  //initialisations
  u4 i=0; //index to Output array
  u4 j=0; //index to Input array

  //transfer the data by shifting and copying
  for ( ; j < nLength; i++, j += 4)
  {
    Output[i] = (u4)Input[j]      | 
          (u4)Input[j+1] << 8 | 
          (u4)Input[j+2] << 16 | 
          (u4)Input[j+3] << 24;
  }
}

const AString& ADigest_MD5::getDigest() const
{
  if (mbool_Finalized)
    return mstr_Digest;
  else
    ATHROW(NULL, AException::InvalidState);
}

void ADigest_MD5::emit(AOutputBuffer& target) const
{
  if (mbool_Finalized)
    ATextConverter::encodeHEX(mstr_Digest, target);
  else
    ATHROW(NULL, AException::InvalidState);
}
