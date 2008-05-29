/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplateNodeHandler_OBJECT_HPP__
#define INCLUDED__ATemplateNodeHandler_OBJECT_HPP__

#include "apiABase.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: CODE
*/
class ABASE_API ATemplateNodeHandler_OBJECT : public ATemplateNodeHandler
{
public:
  /*!
  Tag name of this handler
  */
  static const AString TAGNAME;

public:
  /*!
  ctor
  */
  ATemplateNodeHandler_OBJECT();

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_OBJECT();

  /*!
  Initialize internal resources
  */
  virtual void init();

  /*!
  Release resources
  */
  virtual void deinit();

  /*!
  Name of the tag that this node can handle
  */
  virtual const AString& getTagName() const;

  /*!
  Creator method used for parsing this tag
  */
  virtual ATemplateNode *create(AFile&);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  No copy ctor, each instance is attached to a template
  */
  ATemplateNodeHandler_OBJECT(const ATemplateNodeHandler_OBJECT&) {}

public:
  /*!
  Node class that handles CODE tag
  */
  class ABASE_API Node : public ATemplateNode
  {
  public:
    /*!
    Ctor
    */
    Node(ATemplateNodeHandler *pHandler);
    
    /*!
    Copy ctor
    */
    Node(const ATemplateNodeHandler_OBJECT::Node&);

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    */
    virtual void process(ATemplateContext& context, AOutputBuffer& output);
  
  protected:
    /*!
    Handle the data after it is read (data cleanup, etc)
    Default does nothing, some nodes may need to trim strings, convert, re-format, etc
    */
    virtual void _handleDataAfterRead();
  };
};

#endif //INCLUDED__ATemplateNodeHandler_OBJECT_HPP__
