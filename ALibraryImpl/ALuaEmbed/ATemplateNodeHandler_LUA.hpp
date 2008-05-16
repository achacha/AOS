#ifndef INCLUDED__ATemplateNodeHandler_LUA_HPP__
#define INCLUDED__ATemplateNodeHandler_LUA_HPP__

#include "apiABase.hpp"
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

Handler contains 1 instance of ALuaEmbed which is used by all nodes created by this handler
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
  See ALuaEmbed for the librariesToLoad parameter
  */
  ATemplateNodeHandler_LUA(u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE);

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_LUA();

  /*!
  Add external (user-defined) library to load when Lua is initialized
  */
  void addUserDefinedLibrary(ALuaEmbed::LUA_OPENLIBRARY_FPTR fptr);
  
  /*!
  Initialize internal resources
  Called by ATemplate::init()
  */
  virtual void init();
  
  /*!
  Release resources before caching/storing ATemplate to minimize on memory/resources used
  Will be reallocated as they are needed
  Called by ATemplate::deinit()
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
  Libraries to load when ALuaEmbed is created\
  */
  u4 m_LibrariesToLoad;

  /*!
  Queue up the library methods to call when Lua is initialized
  */
  typedef std::list<ALuaEmbed::LUA_OPENLIBRARY_FPTR> USER_DEFINED_FPTRS;
  USER_DEFINED_FPTRS m_UserDefinedLibFunctions;

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
    virtual void process(ATemplateContext& context);

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  };
};

#endif //INCLUDED__ATemplateNodeHandler_LUA_HPP__
