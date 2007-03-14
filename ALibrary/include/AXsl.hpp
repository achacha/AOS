#ifndef INCLUDED__AXsl_HPP__
#define INCLUDED__AXsl_HPP__

#include "uTypes.hpp"
#include "apiAXsl.hpp"
#include "AEmittable.hpp"
#include "AOutputBuffer.hpp"

class AXSL_API AXsl
{
public:
  AXsl();
  virtual ~AXsl();

  /*!
  Perform XSLT
  */
  int Transform(AEmittable& xml, const AString& xslFilename, AOutputBuffer& result);

private:
  void *mp_InternalData;
};

#endif //INCLUDED__AXsl_HPP__
