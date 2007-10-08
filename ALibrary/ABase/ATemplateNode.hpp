#ifndef INCLUDED_ATemplateNode_HPP_
#define INCLUDED_ATemplateNode_HPP_

#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ATemplate;

class ABASE_API ATemplateNode : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Ctor
  */
  ATemplateNode(ATemplate&);
  
  /*!
  Copy ctor
  */
  ATemplateNode(const ATemplateNode&);

  /*!
  dtor
  */
  virtual ~ATemplateNode();

  /*!
  Creator method signature

  static ATemplateNode* create(AFile&);
  */
  typedef ATemplateNode *(CreatorMethod)(ATemplate&, AFile&);
  typedef CreatorMethod* CreatorMethodPtr;

public:
  /*!
  Process the template node against the data model in the ATemplate
  Output should go to the output buffer in ATemplate
  */
  virtual void process() = 0;

protected:
  ATemplate& m_ParentTemplate;
};

#endif //INCLUDED_ATemplateNode_HPP_