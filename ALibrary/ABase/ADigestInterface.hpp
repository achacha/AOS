#ifndef INCLUDED_ADigestInterface_HPP_
#define INCLUDED_ADigestInterface_HPP_

#include "apiABase.hpp"
#include "AString.hpp"
#include "AEmittable.hpp"

class AFile;

class ABASE_API ADigestInterface : public AEmittable
{
public:
  /*!
  Usage:
  1. call update as many times as needed
  2. call finalize
  3. use getDigest() for binary digest or emit() for ASCII
  4. call reset and start again
  **/
  virtual void reset() = 0;
  virtual void update(AFile&) = 0;
  virtual void update(const AString&, size_t inputLen = AConstant::npos) = 0;  //a_inputLen can be specified for partial buffers
  virtual void finalize() = 0;

  /*!
  Get result AFTER finalize() was called
  */
  virtual const AString& getDigest() const = 0;     //a_Binary form
};

/////////////////////////////////////////////////////////////////////////
// AMD5Digest is an implementation of MD5 algorithm
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

#endif //INCLUDED_AMD5Digest_HPP_
