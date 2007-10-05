#ifndef INCLUDED__ATemplate_HPP__
#define INCLUDED__ATemplate_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "ATemplateNode.hpp"

/*!
Generic template parser/processor

Tag format: {TAG_WRAPPER}{TAG}{TAR_WRAPPER}{BLOCK_START} .. tag body .. {BLOCK_END}{TAG_WRAPPER}{TAG}{TAR_WRAPPER}
Example:    %%CODE%%{ print(/foo); }%%CODE%%
*/
class ABASE_API ATemplate : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  /*!
  Delimiters
  */
  static const AString TAG_WRAPPER;   //a_ "%%"
  static const AString BLOCK_START;   //a_ "{"
  static const AString BLOCK_END;     //a_ "}"

public:
  ATemplate();
  virtual ~ATemplate();

  /*!
  Output based on a given source XML root element
  */
  virtual void process(AOutputBuffer&, const AXmlElement&);

  /*!
  AXmlEmittable, emits the template
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  AEmittable, emits the template
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  ASerializable
  Reading and writing the actual template
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  Class used as a static global to register a creator with template
  */
  class RegisterWithTemplateParser
  {
  public:
    RegisterWithTemplateParser(const AString& tagName, ATemplateNode::CreatorMethodPtr);
  private:
    RegisterWithTemplateParser() {}
  };

protected:
  /*!
  Register tag creator methods
  ATemplateNode_Text is a special case and does not need to be registered
  */
  static void _registerCreator(const AString& tagName, ATemplateNode::CreatorMethodPtr);

private:
  //a_Creator method map
  typedef std::map<AString, ATemplateNode::CreatorMethodPtr> MAP_CREATORS;
  static MAP_CREATORS m_Creators;
  
  //a_Parsed nodes
  typedef std::list<ATemplateNode *> NODES;
  NODES m_Nodes;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#define ATEMPLATE_REGISTER_NODE(tagname, classname) static ATemplate::RegisterWithTemplateParser s_classname(AString(tagname), classname::create);

#endif //INCLUDED__ATemplate_HPP__