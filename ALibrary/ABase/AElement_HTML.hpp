#ifndef INCLUDED__AElementHTML_HPP__
#define INCLUDED__AElementHTML_HPP__

#include "apiABase.hpp"
#include "AElementInterface.hpp"
#include "ANameValuePair.hpp"

class ABASE_API AElement_HTML : public AElementInterface
{
public:
  typedef std::list<AElementInterface *> LIST_AElementInterface;

public:
  AElement_HTML(AElementInterface *elementParent = NULL);
  virtual ~AElement_HTML();

 //a_Parsing method
  virtual bool parse(const AString &strInput, size_t &position);
  
  //a_Access to text
  AString &useText() { return m__strText; }

  //a_Access to NAME=VALUE pairs
  bool isName(const AString& strName) const { return ((_findName(strName) != -1) ? TRUE : FALSE); }
  bool getValue(const AString& strName, AString& strValue) const;
  void setNameValue(const AString& strName, const AString& strValue = AConstant::ASTRING_EMPTY);
  void changeName(const AString& strOldName, const AString& strNewName); 
  void removeName(const AString& strName);

  //a_If pheStartAfter is NULL, the current element is returned
  //a_If pheStartAfter is this, then it gives the first child
  //a_This interface is easy to use for inclusive while() iteration after a find
  //a_as in:  while (pheElement = nextChildElement(pheElement)) { ... }
  AElement_HTML *nextChildElement(AElement_HTML *pheStartAfter = NULL);
  
  //a_If pheStartAfter is NULL, the iteration starts at this element and iterates down
  AElement_HTML *findType(const AString &strType, AElement_HTML *pheStartAfter = NULL);
  AElement_HTML *findFormItem(AElement_HTML *pheStartAfter = NULL);
  
  //a_Finds a parent node of a given type
  AElement_HTML *findParentType(const AString& strType);

  //a_Output method
  virtual AString begin(size_t iDepth = AConstant::npos) const; //a_Start of this tag (recursive though all the children)
  virtual AString end(size_t iDepth = AConstant::npos) const;   //a_End of this tag
  AString beginElement() const;                 //a_HTML element specific output (non-recursive)

  //a_HTML Helper methods
  static bool isSingularType(const AString &strType);
  static bool isFormatCritical(const AString &strType);
  static bool isFormItemType(const AString &strType);
  static bool isIWTemplateTag(const AString &strType);

  //a_Extended error reporting
  void setExtendedErrorLogging(bool bFlag = true);

  //a_Display recursively self and children
  //a_This method takes a reference to make recursive calls more efficient
  //a_Returning an string would stress the stack too much in recursive calls
  //a_iIndentLevel of -1 means do not indent or format
  //a_iIndentLevel >= 0  indents two spaces for each level and adds CRLF
  void recurseToString(AString& strReturn, int iIndentLevel = -1) const;
  
  //a_If false is returned, it has no parent and cannot kill self
  void removeChild(AElement_HTML *peChild);

	//a_Cleanup
	virtual void clear();

protected:
  //a_NAME=VALUE elements of this HTML element
  VECTOR_NVPairPtr m_vectorPairs;
  size_t _findName(const AString &strName) const;     //a_Finds the index to a name/value in the vector

  //a_Creation method
  virtual AElementInterface *_create(AElementInterface *pParent);

  //a_Children
  LIST_AElementInterface m_listChildElements;
  virtual AElement_HTML *_addChild(AElementInterface *peChild);

  //a_Locating a non-singular parent
  AElement_HTML *_findNonSingularParent();

private:
  static const AString PARSE_END_ELEMENT;  // "/>"
  static char PARSE_START_ELEMENT;         // '<'

  //a_Parsing helper methods
  void __parseTextBlock(const AString &strInput, size_t& position);
  void __parseNameValuePairs(const AString &strInput, size_t& position);
  void __parseAdvanceToNextTag(const AString &strInput, size_t& position);
  void __parseAdvanceToEndOfTag(const AString &strInput, size_t& position);
  void __parseSkipOverWhiteSpace(const AString &strInput, size_t& position);
  void __parseGetType(const AString &strInput, size_t& position);
  
  //a_Status of parsing
  bool __isInsideFormatCriticalBlock();

  //a_If this is a text block, this will contain the text data and TYPE will be empty
  AString m__strText;

  //a_Reset this element
  void __reset();

  //a_Flags
  bool m__boolExtendedErrorLogging;

  //a_Destruction
  void __destroy();

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

