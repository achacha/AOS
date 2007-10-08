#ifndef INCLUDED_ATemplateNode_Code_HPP_
#define INCLUDED_ATemplateNode_Code_HPP_

#include "ATemplateNode.hpp"
#include "AFile_AString.hpp"

/*!
Simple example template node

Handles %%CODE%%{ code goes here }%%CODE%% tag

Evaluates ATemplate::OBJECTNAME_MODEL object which contains AXmlDocument

Supports:
  print(<xml path>) - displays content for the goven path
  count(<xml path>) - counts elements under the immediate given path

One instruction per line and terminated by ;

Example of a CODE block:
%%CODE%%{
print(/root/myelement/data);
count(/root);
}%%CODE%%

Sample code:
  AFile_AString strfile("\
Hello %%CODE%%{\r\n print(/root/user/name); print(/root/user/loc); \r\n}%%CODE%%!\
");

  AAutoPtr<AXmlDocument> pdoc(new AXmlDocument("root"));  // Some xml model document (add data here or this may already exist)
  ARope rope;  // Something to get the output

  ATemplate templt(pdoc, &rope);
  ATEMPLATE_USE_NODE(templt, ATemplateNode_Code);  //a_Tells this instance that it can handle %%CODE%% types
  templt.fromAFile(strfile);
  templt.process();
  std::cout << templt.useOutput() << std::endl;

*/
class ABASE_API ATemplateNode_Code : public ATemplateNode
{
public:
  static const AString TAGNAME; //a_ "CODE"

public:
  /*!
  Copy ctor
  */
  ATemplateNode_Code(const ATemplateNode_Code&);

  /*!
  Tag name accessor
  */
  virtual const AString& getTagName() const;

  /*!
  Generate output evaluated against AObjectContainer
  */
  virtual void process();

  /*
  true if some code in here, else false
  */
  bool isEmpty() const;

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
  //a_Use create()
  ATemplateNode_Code(ATemplate&);

private:
  LIST_AString m_Lines;

  //a_Process helpers
  void _processLine(const AString&);

  //a_Delimeter
  static const AString sstr_LineDelimeter; //a_ ";"

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ATemplateNode_Code_HPP_
