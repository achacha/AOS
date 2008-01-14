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
  */
  virtual void reset() = 0;
  virtual void update(AFile&) = 0;
  virtual void update(const AString&, size_t inputLen = AConstant::npos) = 0;  //a_inputLen can be specified for partial buffers
  virtual void finalize() = 0;

  /*!
  Get result AFTER finalize() was called
  */
  virtual const AString& getDigest() const = 0;     //a_Binary form
};

#endif //INCLUDED_AMD5Digest_HPP_
