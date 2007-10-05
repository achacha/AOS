#ifndef INCLUDED_ATemplateNode_HPP_
#define INCLUDED_ATemplateNode_HPP_

#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ATemplateNode : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Creator method signature

  static ATemplateNode* create(AFile&);
  */
  typedef ATemplateNode *(CreatorMethod)(AFile&);
  typedef CreatorMethod* CreatorMethodPtr;

public:
  /*!
  Process the template node against the data model
  */
  virtual void process(AOutputBuffer&, const AXmlElement&) = 0;
};

#endif //INCLUDED_ATemplateNode_HPP_