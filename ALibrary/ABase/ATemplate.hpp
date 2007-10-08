#ifndef INCLUDED__ATemplate_HPP__
#define INCLUDED__ATemplate_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "ATemplateNode.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlDocument.hpp"
#include "ABasePtrHolder.hpp"
#include "templateAutoPtr.hpp"

/*!
Generic template parser/processor

Tag format: {START_TAG}{TAG}{BLOCK_START} .. tag body .. {BLOCK_END}{TAG}{TAR_END}
Example:    #[CODE]!{ print(/foo); }![CODE]#
*/
class ABASE_API ATemplate : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  /*!
  Object names used to lookup ABase* types
  */
  static const AString OBJECTNAME_MODEL;   // AXmlDocument that acts as a model for template lookup

  /*!
  Delimiters
  */
  static const AString TAG_START;     //a_ "#["
  static const AString BLOCK_START;   //a_ "]!{"
  static const AString BLOCK_END;     //a_ "}!["
  static const AString TAG_END;       //a_ "]#"

public:
  /*!
  Ctor with optional AXmlDocument and AOutputBuffer to be used as a model for this template expansion
  If output is NULL, the default internal one is used

  NOTE: Objects passed to ctor are NOT owned and will NOT be deleted
  */
  ATemplate(AXmlDocument *pModel, AOutputBuffer *pOutput = NULL);
  
  /*!
  dtor
  */
  virtual ~ATemplate();

  /*!
  Output based on a given source XML root element
  */
  virtual void process();

  /*!
  Access to the data model objects used in evaluation
  OBJECTNAME_MODEL is where the AXmlDocument used for evaluation is stored
  */
  ABasePtrHolder& useObjects();

  /*!
  Data model
  */
  AXmlDocument& useModel();

  /*!
  Output buffer stored in the holder
  */
  AOutputBuffer& useOutput();

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
  class ABASE_API RegisterWithTemplateParser
  {
  public:
    RegisterWithTemplateParser(const AString& tagName, ATemplateNode::CreatorMethodPtr);
  private:
    RegisterWithTemplateParser() {}
  };

  /*!
  Register tag creator methods
  ATemplateNode_Text is a special case and does not need to be registered
  */
  void registerCreator(const AString& tagName, ATemplateNode::CreatorMethodPtr);

private:
  // No default ctor
  ATemplate();

  //a_Object pointers used during template evaluation
  ABasePtrHolder m_Objects;

  //a_Output buffer
  AAutoPtr<AOutputBuffer> mp_Output;

  //a_Creator method map
  typedef std::map<AString, ATemplateNode::CreatorMethodPtr> MAP_CREATORS;
  MAP_CREATORS m_Creators;
  
  //a_Parsed nodes
  typedef std::list<ATemplateNode *> NODES;
  NODES m_Nodes;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#define ATEMPLATE_USE_NODE(templt, classname) do {\
  templt.registerCreator(classname::TAGNAME, classname::create);\
} while(0)


#endif //INCLUDED__ATemplate_HPP__