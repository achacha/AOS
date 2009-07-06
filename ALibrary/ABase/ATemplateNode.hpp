/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplateNode_HPP__
#define INCLUDED__ATemplateNode_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AXmlAttributes.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ATemplate;
class ATemplateContext;
class ATemplateNodeHandler;
class ABasePtrContainer;

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
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

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
  Access the attributes defined for this specific node
  
  @return AXmlAttributes object for this node
  */
  AXmlAttributes& useAttributes();

  /*!
  Access the attributes defined for this specific node
  
  @return const AXmlAttributes object for this node
  */
  const AXmlAttributes& getAttributes() const;

  /*!
  Process the template node that contains the data for this tag to handle

  @param context of the execution
  @param output buffer
  */
  virtual void process(ATemplateContext& context, AOutputBuffer& output);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Handle the data after it is read (data cleanup, etc)
  Default does nothing, some nodes may need to trim strings, convert, re-format, etc
  */
  virtual void _handleDataAfterRead();

  //a_Block data
  AString m_BlockData;

  //a_Attributes specified as part of the node
  AXmlAttributes m_Attributes;

  //a_Handler of this node
  ATemplateNodeHandler *mp_Handler;
};

#endif //INCLUDED__ATemplateNode_HPP__
