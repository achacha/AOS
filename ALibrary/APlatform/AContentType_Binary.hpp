/*
Written by Alex Chachanashvili

$Id: AContentType_Binary.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED__AContentType_Binary_HPP__
#define INCLUDED__AContentType_Binary_HPP__

#include "apiAPlatform.hpp"
#include "AContentTypeInterface.hpp"

class AOutputBuffer;

class APLATFORM_API AContentType_Binary : public AContentTypeInterface
{
public:
  static const AString CONTENT_TYPE;  // application/octet-stream

public:
  AContentType_Binary();
  virtual ~AContentType_Binary();

  //a_Activation methods do nothing, binary data stores all types and knows nothing about them
  virtual void parse();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;  
};

#endif // INCLUDED__AContentType_Binary_HPP__

