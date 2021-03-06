/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplateNodeHandler_HPP__
#define INCLUDED__ATemplateNodeHandler_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

class AOutputBuffer;
class ATemplateNode;
class ABasePtrContainer;
class AFile;

/*!
Handler for a tag to register with template and process node

One handler is registered with ATemplate 
This handler is then used to create ATemplateNote types associated with this handler

Derived classes should have a TAGNAME public member for convenience
*/
class ABASE_API ATemplateNodeHandler : public ADebugDumpable
{
public:
  /*!
  ctor
  */
  ATemplateNodeHandler();
  
  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler();

  /*!
  Initialize internal resources
  */
  virtual void init() = 0;

  /*!
  Release resources
  */
  virtual void deinit() = 0;

  /*!
  Name of the tag that this node can handle
  */
  virtual const AString& getTagName() const = 0;

  /*!
  Creator method used for handling this tag
  If AFile is NULL then empty node is created
  */
  virtual ATemplateNode *create(AFile *pFile = NULL) = 0;

  /*!
  Clone self
  */
  virtual ATemplateNodeHandler *clone() = 0;

protected:
  /*!
  No copy ctor, each instance is attached to a template
  */
  ATemplateNodeHandler(const ATemplateNodeHandler&) {}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplateNodeHandler_HPP__
