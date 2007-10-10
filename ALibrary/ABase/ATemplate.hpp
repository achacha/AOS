#ifndef INCLUDED__ATemplate_HPP__
#define INCLUDED__ATemplate_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AXmlEmittable.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"
#include "AOutputBuffer.hpp"
#include "ABasePtrHolder.hpp"
#include "templateAutoPtr.hpp"

class ABasePtrHolder;

/*!
Generic template parser/processor

Tag format: {START_TAG}{TAG}{BLOCK_START} .. tag body .. {BLOCK_END}{TAG}{TAR_END}
Example:    %[CODE]%{{{ print(/foo); }}}%[CODE]%

Usage:
  // Create a template and add handlers
  ATemplate t;
  t.addHandler(new ATemplateNodeHandler_Code(t));
  t.addHandler(new ATemplateNodeHandler_SomeCustomOne(t));

  // Setup an objects holder to be used by the template
  ABasePtrHolder objects;
  AXmlDocument doc("root");
  
  // This is a well known object and will be used to expand template parts in ATemplateNodeHandler_Code and others
  // Handlers can make changes to this since it is just a reference
  objects.insert(ATemplate::OBJECTNAME_MODEL, &doc);
  
  // Something else that may be used in template expansion and discarded when ATemplate is out of scope
  objects.insertWithOwnership("someobject", new AString("foo"));
  
  //process template nodes using the model
  t.process(objects);

  //Display results
  std::cout << t.getOutput() << std::endl;
*/
class ABASE_API ATemplate : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  /*!
  Well known object name
  */
  static const AString OBJECTNAME_MODEL;      //a_This is where AXmlDocument* will be stored in ABasePtrHolder
  
  /*!
  Delimiters
  */
  static const AString TAG_START;     //a_ "%["
  static const AString BLOCK_START;   //a_ "]{{{"
  static const AString BLOCK_END;     //a_ "}}}%["
  static const AString TAG_END;       //a_ "]%"

public:
  /*!
  Ctor
  */
  ATemplate();
  
  /*!
  dtor
  */
  virtual ~ATemplate();

  /*!
  Output based on a given source XML root element and output buffer
  Evaluate template against a model and objects
  */
  virtual void process(ABasePtrHolder& objects, AOutputBuffer& output);

  /*!
  AXmlEmittable, emits the template
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  AEmittable, emits the template
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  ASerializable
  Reading and writing the actual template
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  Attach a node handler
  This object will OWN and DELETE the handler when done with it
  */
  void addHandler(ATemplateNodeHandler *);

private:
  //a_Parsed nodes
  typedef std::list<ATemplateNode *> NODES;
  NODES m_Nodes;

  //a_Handlers
  typedef std::map<AString, AAutoPtr<ATemplateNodeHandler>> HANDLERS;
  HANDLERS m_Handlers;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplate_HPP__
