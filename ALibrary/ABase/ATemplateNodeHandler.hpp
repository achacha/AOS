#ifndef INCLUDED__ATemplateNodeHandler_HPP__
#define INCLUDED__ATemplateNodeHandler_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

class AOutputBuffer;
class ATemplateNode;
class ABasePtrHolder;
class AFile;

/*!
Handler for a tag to register with template and process node
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
  */
  virtual ATemplateNode *create(AFile&) = 0;

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
