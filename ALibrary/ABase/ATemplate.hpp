#ifndef INCLUDED__ATemplate_HPP__
#define INCLUDED__ATemplate_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"

class ATemplateNode;

class ABASE_API ATemplate : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  //a_Delimeters
  static const AString sstr_CodeStart;   //a_ "<!--["
  static const AString sstr_CodeEnd;     //a_ "]-->"

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
  virtual void emit(AXmlElement&) const;

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

private:
  typedef std::list<ATemplateNode *> NODES;
  NODES m_Nodes;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATemplate_HPP__