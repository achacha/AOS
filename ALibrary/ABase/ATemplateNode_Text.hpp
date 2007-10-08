#ifndef INCLUDED_ATemplateNode_Text_HPP_
#define INCLUDED_ATemplateNode_Text_HPP_

#include "ATemplateNode.hpp"
#include "AString.hpp"

/*!
Special node that used to hold text between tags
Does NOT need to be registered
*/
class ATemplateNode_Text : public ATemplateNode
{
public:
  /*!
  ctor
  Special case: this node class is used by ATemplate directly and thus not registered
  */
  ATemplateNode_Text(ATemplate&, const AString& text = AConstant::ASTRING_EMPTY);
  
  /*!
  Copy ctor
  */
  ATemplateNode_Text(const ATemplateNode_Text&);

  /*!
  Tag name accessor
  Not really applicable for this node type, returns empty string
  */
  virtual const AString& getTagName() const;

  /*!
  Generate output
  */
  virtual void process();

  /*!
  Access to the test
  */
  AString& useText();

  /*!
  AXmlEmittable, emits the template
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

private:
  AString m_Text;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_Text_HPP_
