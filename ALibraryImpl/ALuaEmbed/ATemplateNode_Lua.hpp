#ifndef INCLUDED_ATemplateNode_Lua_HPP_
#define INCLUDED_ATemplateNode_Lua_HPP_

#include "ATemplateNode.hpp"

/*!
Handles %%LUA%%{ code goes here }%%LUA%% tag
*/
class ATemplateNode_Lua : public ATemplateNode
{
public:
  /*!
  Copy ctor
  */
  ATemplateNode_Lua(const ATemplateNode_Lua&);

  /*!
  Generate output evaluated against TODO: MAP_AObjectBase
  */
  virtual void process(AOutputBuffer&, const AXmlElement&);

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
  static ATemplateNode* create(AFile&);

protected:
  //a_No default ctor, use create()
  ATemplateNode_Lua() {}

private:
  AString m_Script;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_Lua_HPP_
