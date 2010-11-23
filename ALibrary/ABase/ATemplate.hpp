/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplate_HPP__
#define INCLUDED__ATemplate_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AXmlEmittable.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"
#include "AOutputBuffer.hpp"
#include "ABasePtrContainer.hpp"
#include "ATemplateContext.hpp"

/*!
Generic template parser/processor

Code Example  : <aos:lua>{{{ print("Hello World"); }}}</aos:lua>    (To use lua, you have to addHandler(new ATemplateNodeHandler_LUA()) to the template)
Model example: <aos:model>{{{ /model/path }}}</aos:model>
Object example: <aos:object>{{{ object_name }}}</aos:object>


Usage:
  // For logging events
  AEventVisitor visitor;

  // Create a template and add handlers
  ATemplate t;
  t.addHandler(new ATemplateNodeHandler_SomeCustomOne(t));

  // Setup an objects holder to be used by the template
  ABasePtrContainer objects;
  AXmlDocument model("root");
  
  // Create a context
  ATemplateContext ctx(objects, model, visitor);
  
  // Something else that may be used in template expansion and discarded when ATemplate is out of scope
  objects.insertWithOwnership("someobject", new AString("foo"));
  
  //process template nodes using the model
  AString output;
  t.process(ctx, output);

  //Display results
  std::cout << output << std::endl;
*/
class ABASE_API ATemplate : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  /*!
  Default handler enums
  */
  enum HandlerMask
  {
    HANDLER_NONE   = 0x00,
    HANDLER_OBJECT = 0x01,
    HANDLER_MODEL  = 0x02,
    HANDLER_ALL    = 0xff
  };

public:
  /*!
  ctor
  Parse given string template

  @param t template to parse
  @param mask loads OBJECT and MODEL handlers by default (others can use addHandler call)
  */
  ATemplate(const AString& t, HandlerMask mask = ATemplate::HANDLER_ALL);

  /*!
  ctor
  Parse given string template

  @param file to read for template to parse
  @param mask loads OBJECT and MODEL handlers by default (others can use addHandler call)
  */
  ATemplate(AFile& file, HandlerMask mask = ATemplate::HANDLER_ALL);

  /*!
  Ctor

  @param mask loads OBJECT and MODEL handlers by default (others can use addHandler call)
  */
  ATemplate(HandlerMask mask = ATemplate::HANDLER_ALL);
  
  /*!
  dtor
  */
  virtual ~ATemplate();

  /*!
  Add node for an existing handler to the template
  This is used to manually build a template from node data
  */
  void addNode(const AString& tagname, AFile& source);

  /*!
  Clear the template
  */
  void clear();

  /*!
  Initializes handlers if they are hibernated
  Output based on a given source XML root element and output buffer
  Evaluate template against a model and objects
  */
  virtual void process(
    ATemplateContext& context,
    AOutputBuffer& output
  );

  /*!
  AXmlEmittable, emits the template
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

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
  Helper function that takes AString and uses AFile_AString to call fromAFile
  
  @param source AString data
  */
  void ATemplate::fromAString(const AString& source);
  
  /*!
  Attach a node handler
  This object will OWN and DELETE the handler when done with it

  Example:  tmpl.addHandler(new ATemplateNodeHandler_CUSTOM())
  */
  void addHandler(ATemplateNodeHandler *);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! Initialize handlers
  void _initializeHandlers();

  //! Deinitialize handlers
  void _deinitializeHandlers();

  //! Load deafult handlers
  void _loadDefaultHandlers(ATemplate::HandlerMask mask);

  //! Parsed nodes
  typedef std::list<ATemplateNode *> NODES;
  NODES m_Nodes;

  //! Handlers
  typedef std::map< AString, ATemplateNodeHandler* > HANDLERS;
  HANDLERS m_Handlers;
};

#endif //INCLUDED__ATemplate_HPP__
