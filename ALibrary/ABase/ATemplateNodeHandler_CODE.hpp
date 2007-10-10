#ifndef INCLUDED__ATemplateNodeHandler_CODE_HPP__
#define INCLUDED__ATemplateNodeHandler_CODE_HPP__

#include "apiABase.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: CODE
*/
class ABASE_API ATemplateNodeHandler_CODE : public ATemplateNodeHandler
{
public:
  /*!
  ctor
  */
  ATemplateNodeHandler_CODE();

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_CODE();

  /*!
  Name of the tag that this node can handle
  */
  virtual const AString& getTagName() const;

  /*!
  Creator method used for parsing this tag
  */
  virtual ATemplateNode *create(AFile&);

protected:
  /*!
  No copy ctor, each instance is attached to a template
  */
  ATemplateNodeHandler_CODE(const ATemplateNodeHandler_CODE&) {}

  /*!
  Tag name of this handler
  */
  static const AString TAGNAME;

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
    Node(const ATemplateNodeHandler_CODE::Node&);

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    Output should go to the output buffer in ATemplate
    */
    virtual void process(ABasePtrHolder& objects, AOutputBuffer& output);

  protected:
    void _processLine(const AString&, AXmlElement&, AOutputBuffer&);
  };

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplateNodeHandler_CODE_HPP__
