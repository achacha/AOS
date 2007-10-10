#ifndef INCLUDED__ATemplateNodeHandler_LUA_HPP__
#define INCLUDED__ATemplateNodeHandler_LUA_HPP__

#include "apiABase.hpp"
#include "ALuaEmbed.hpp"
#include "ATemplateNode.hpp"
#include "ATemplateNodeHandler.hpp"

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
  ctor
  See ALuaEmbed for the librariesToLoad parameter
  */
  ATemplateNodeHandler_LUA(u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE);

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
  virtual ATemplateNode *create(AFile&);

  /*!
  Access to Lua interpreter
  */
  ALuaEmbed& useLua();

protected:
  /*!
  Tag name of this handler
  */
  static const AString TAGNAME;

  /*!
  Lua embedded interpreter
  */
  ALuaEmbed m_Lua;

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
    Output should go to the output buffer in ATemplate
    */
    virtual void process(ABasePtrHolder& objects, AOutputBuffer& output);

  public:
  #ifdef __DEBUG_DUMP__
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  #endif
  };

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplateNodeHandler_LUA_HPP__
