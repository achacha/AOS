#ifndef INCLUDED_ATemplateNode_HPP_
#define INCLUDED_ATemplateNode_HPP_

#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ATemplateNode : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  virtual void process(AOutputBuffer&, const AXmlElement&) = 0;

  /*!
  AXmlEmittable
  AEmittable
  */
  void emit(AOutputBuffer&) const = 0;
  void emit(AXmlElement&) const = 0;
};

#endif //INCLUDED_ATemplateNode_HPP_