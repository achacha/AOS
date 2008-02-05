#ifndef INCLUDED__ADadaDataHolder_HPP__
#define INCLUDED__ADadaDataHolder_HPP__

#include "AString.hpp"
#include "ADebugDumpable.hpp"

class AFilename;
class AOSServices;

class ADadaDataHolder : public ADebugDumpable, public AXmlEmittable
{
public:
  ADadaDataHolder();
  ADadaDataHolder(const ADadaDataHolder&);
  virtual ~ADadaDataHolder();

  typedef std::map<AString, VECTOR_AString> WORDMAP;
  typedef std::map<AString, VECTOR_AString> TEMPLATES;

  WORDMAP& useWordMap() { return m_wordmap; }
  TEMPLATES& useTemplates() { return m_templates; }

//  void readData(const AFilename& iniFilename, const AFilename& baseDataDir,  ALog&);
  void readData(AOSServices& services, const AXmlElement *pSet);
  void resetData();

  /*
  AObjectBase guarantees
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  WORDMAP m_wordmap;
  TEMPLATES m_templates;

  void _loadWords(const AString& type, const AFilename& filename, ALog&);
  void _loadTemplate(const AString& name, const AFilename& filename, ALog&);
};

#endif // INCLUDED__ADadaDataHolder_HPP__
