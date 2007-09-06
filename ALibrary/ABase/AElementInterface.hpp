#ifndef INCLUDED__AElementInterface_HPP__
#define INCLUDED__AElementInterface_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"

class ABASE_API AElementInterface : public ADebugDumpable
{
public:
  //a_Every element must have a parent, even if NULL
  AElementInterface(AElementInterface *pelementParent = NULL);
  virtual ~AElementInterface() {};

  //a_Each element must have an ability to parse itself
  //a_Given a string {strInput} at string offset {iPosition}
  //a_Returns if should continue parsing
  virtual bool parse(const AString &strInput, size_t &position) = 0;

  //a_Output method (depth controls indentation) (-1 means ignore)
  virtual AString begin(size_t iDepth = AConstant::npos) const = 0;         //a_Start of this tag
  virtual AString end(size_t iDepth = AConstant::npos)   const = 0;         //a_End of this tag

  //a_Utility functions
  virtual bool isWhiteSpace(char cX) const { return ((AConstant::ASTRING_WHITESPACE.find(cX) == AConstant::npos) ? false : true); }

  //a_Singularity methods
  bool isSingular() const { return m__boolSingular; }
  void setSingular(bool boolFlag = TRUE) { m__boolSingular = boolFlag; }

  //a_Type of this element (always lower case)
  virtual const AString& getType() const { return m_strType; }
  virtual void           setType(const AString& strType) { m_strType = strType; m_strType.makeLower(); }
  virtual bool           isType(const AString& strType) { return (m_strType.compareNoCase(strType) ? false : true); }

  //a_Access to parent
  AElementInterface *getParent() const { return m__pelementParent; }

	//a_Cleanup
	void reset();

protected:
  //a_Creation method
  virtual AElementInterface *_create(AElementInterface *pParent) = 0;

  //a_Access to the type by child classes
  AString& _getType() { return m_strType; }

  //a_Access to parent
  void _setParent(AElementInterface *pelementParent) { m__pelementParent = pelementParent; }
  bool _isChild(AElementInterface *peChild);

  //a_Type <TYPE NAME=VALUE...> or similar
  AString m_strType;

private:
  //a_Singular objects need not call end()
  bool m__boolSingular;

  //a_Pointer to the parent node
  AElementInterface *m__pelementParent;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

