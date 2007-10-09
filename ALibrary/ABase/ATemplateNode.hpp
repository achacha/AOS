#ifndef INCLUDED__ATemplateNode_HPP__
#define INCLUDED__ATemplateNode_HPP__

#include "apiABase.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"

class ATemplate;
class ATemplateNodeHandler;
class ABasePtrHolder;

/*!
Base template node
*/
class ABASE_API ATemplateNode : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Ctor
  Assign a handler to this node, if NULL then it will do nothing when processed (handler 
  */
  ATemplateNode(ATemplateNodeHandler *pHandler = NULL);
  
  /*!
  Copy ctor
  */
  ATemplateNode(const ATemplateNode&);

  /*!
  dtor
  */
  virtual ~ATemplateNode();

  /*!
  Name of the tag that this node handles, empty string if no handler
  */
  const AString& getTagName() const;
  
  /*
  String buffer that holds the script
  */
  AString& useBlockData();

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
  Process the template node that contains the data for this tag to handle
  Output should go to the output buffer in ATemplate
  */
  virtual void process(ABasePtrHolder& objects, AOutputBuffer& output);

protected:
  //a_Block data
  AString m_BlockData;

  //a_Handler of this node
  ATemplateNodeHandler *mp_Handler;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplateNode_HPP__
