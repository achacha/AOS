#ifndef INCLUDED_ATemplateNode_Lua_HPP_
#define INCLUDED_ATemplateNode_Lua_HPP_

#include "ATemplateNode.hpp"
#include "ATemplate.hpp"

/*!
Handles %%LUA%%{ code goes here }%%LUA%% tagname
*/
class ALuaEMBED_API ATemplateNode_Lua : public ATemplateNode
{
public:
  static const AString TAGNAME; //a_ "LUA" used when parsing tokens in a template

public:
  /*!
  Copy ctor
  */
  ATemplateNode_Lua(const ATemplateNode_Lua&);

  /*!
  dtor
  */
  virtual ~ATemplateNode_Lua();

  /*!
  Generate output evaluated against TODO: MAP_AObjectBase
  */
  virtual void process();

  /*
  String buffer that holds the script
  */
  const AString& getScript() const;

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  AEmittable, emits the template
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Read/save this node
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  Creator method
  */
  static ATemplateNode* create(ATemplate&, AFile&);

protected:
  /*!
  Ctor
  */
  ATemplateNode_Lua(ATemplate&);

private:
  //a_Script to execute
  AString m_Script;
  
  //a_Name of the object that will be inserted into the object holder of the parent template
  //a_ that will contain an instance of Lua interpreter
  static const AString ATemplateNode_Lua::OBJECTNAME_LUA;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_Lua_HPP_
