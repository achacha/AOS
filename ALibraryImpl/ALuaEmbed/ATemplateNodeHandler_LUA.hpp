/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplateNodeHandler_LUA_HPP__
#define INCLUDED__ATemplateNodeHandler_LUA_HPP__

#include "apiALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"
#include "templateAutoPtr.hpp"
#include "ASync_CriticalSection.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: LUA
*/
class ALuaEMBED_API ATemplateNodeHandler_LUA : public ATemplateNodeHandler
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
  ATemplateNodeHandler_LUA();

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_LUA();
  
  /*!
  Name of the tag that this node can handle
  */
  virtual const AString& getTagName() const;

  /*!
  Creator method used for parsing this tag
  */
  virtual ATemplateNode *create(AFile *pFile = NULL);

  /*!
  Initialize internal resources
  */
  virtual void init();

  /*!
  Release resources
  */
  virtual void deinit();

  /*!
  Clone
  */
  virtual ATemplateNodeHandler *clone();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

public:
  /*!
  Node class that handles CODE tag
  */
  class ALuaEMBED_API Node : public ATemplateNode
  {
  public:
    /*!
    Ctor
    */
    Node(ATemplateNodeHandler *pHandler);
    
    /*!
    Copy ctor
    */
    Node(const ATemplateNodeHandler_LUA::Node&);    

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    Output should go to the output buffer
    */
    virtual void process(ATemplateContext& context, AOutputBuffer& output);

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  };
};

#endif //INCLUDED__ATemplateNodeHandler_LUA_HPP__
