#ifndef INCLUDED_ATemplateNode_Code_HPP_
#define INCLUDED_ATemplateNode_Code_HPP_

#include "ATemplateNode.hpp"
#include "AFile_AString.hpp"

class ATemplateNode_Code : public ATemplateNode
{
public:
  ATemplateNode_Code();
  ATemplateNode_Code(const AString& path);
  ATemplateNode_Code(const ATemplateNode_Code&);

  /*!
  Generate output evaluated against AObjectContainer
  */
  virtual void process(AOutputBuffer&, const AXmlElement&);

  /*
  true if some code in here, else false
  */
  bool isEmpty() const;

  /*!
  AXmlEmittable
  */
  virtual void emit(AXmlElement&) const;

  /*!
  AEmittable, emits the template
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Read/save this node
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

private:
  LIST_AString m_Lines;

  //a_Process helpers
  void _processLine(AOutputBuffer&, const AString&, const AXmlElement&);

  //a_Delimeter
  static const AString sstr_LineDelimeter; //a_ ";"

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_Code_HPP_
