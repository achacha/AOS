/*
Written by Alex Chachanashvili

$Id: ATemplateNodeHandler_RESOURCE.hpp 225 2008-06-13 23:06:13Z achacha $
*/
#ifndef INCLUDED__ATemplateNodeHandler_RESOURCE_HPP__
#define INCLUDED__ATemplateNodeHandler_RESOURCE_HPP__

#include "apiAOS_Base.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: CODE
*/
class AOS_BASE_API ATemplateNodeHandler_RESOURCE : public ATemplateNodeHandler
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
  ATemplateNodeHandler_RESOURCE();

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_RESOURCE();

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
  ATemplateNodeHandler_RESOURCE(const ATemplateNodeHandler_RESOURCE&) {}

public:
  /*!
  Node class that handles CODE tag
  */
  class AOS_BASE_API Node : public ATemplateNode
  {
  public:
    /*!
    Ctor
    */
    Node(ATemplateNodeHandler *pHandler);
    
    /*!
    Copy ctor
    */
    Node(const ATemplateNodeHandler_RESOURCE::Node&);

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    Output should go to the output buffer in ATemplate
    */
    virtual void process(ATemplateContext& objects, AOutputBuffer& output);
  };
};

#endif //INCLUDED__ATemplateNodeHandler_RESOURCE_HPP__
