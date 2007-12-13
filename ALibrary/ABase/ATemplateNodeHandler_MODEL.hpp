#ifndef INCLUDED__ATemplateNodeHandler_MODEL_HPP__
#define INCLUDED__ATemplateNodeHandler_MODEL_HPP__

#include "apiABase.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: CODE
*/
class ABASE_API ATemplateNodeHandler_MODEL : public ATemplateNodeHandler
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
  ATemplateNodeHandler_MODEL();

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_MODEL();

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

protected:
  /*!
  No copy ctor, each instance is attached to a template
  */
  ATemplateNodeHandler_MODEL(const ATemplateNodeHandler_MODEL&) {}

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
    Node(const ATemplateNodeHandler_MODEL::Node&);

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    Output should go to the output buffer in ATemplate
    */
    virtual void process(ABasePtrHolder& objects, AOutputBuffer& output);
  };

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplateNodeHandler_MODEL_HPP__
