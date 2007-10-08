#ifndef INCLUDED_ATemplateNode_HPP_
#define INCLUDED_ATemplateNode_HPP_

#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"

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
  Name of the tag that this node handles
  */
  virtual const AString& getTagName() const = 0;
  
  /*
  String buffer that holds the script
  */
  const AString& getBlockData() const;

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  AEmittable, emits the template
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Read/save this node (ASerializable interface)
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  Process the template node against the data model in the ATemplate
  Output should go to the output buffer in ATemplate
  */
  virtual void process() = 0;

  /*!
  Creator method signature

  static ATemplateNode* create(AFile&);
  */
  typedef ATemplateNode *(CreatorMethod)(ATemplate&, AFile&);
  typedef CreatorMethod* CreatorMethodPtr;

protected:
  //a_Parent template
  ATemplate& m_ParentTemplate;

  //a_Block data
  AString m_BlockData;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_HPP_
