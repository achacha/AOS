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
  Tag name accessor
  */
  virtual const AString& getTagName() const;

  /*!
  Generate output evaluated against TODO: MAP_AObjectBase
  */
  virtual void process();

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
  //a_Name of the object that will be inserted into the object holder of the parent template
  //a_ that will contain an instance of Lua interpreter
  static const AString ATemplateNode_Lua::OBJECTNAME_LUA;
};

#endif //INCLUDED_ATemplateNode_Lua_HPP_
