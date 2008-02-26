#ifndef INCLUDED__ADadaDataHolder_HPP__
#define INCLUDED__ADadaDataHolder_HPP__

#include "AString.hpp"
#include "ADebugDumpable.hpp"

class AFilename;
class AOSServices;

/*!
<AOS_DadaData>
	<dataset name="dada">
		<data type="sometype">path/from/data/path/words.dat</data>
    ...
	</dataset>
  <template name="test">path/from/data/path/templates/test.ddt</template>
  ...
</AOS_DadaData>

*/
class ADadaDataHolder : public ADebugDumpable, public AXmlEmittable
{
public:
  ADadaDataHolder();
  ADadaDataHolder(const ADadaDataHolder&);
  virtual ~ADadaDataHolder();

  typedef std::map<AString, VECTOR_AString> WORDMAP;

  WORDMAP& useWordMap() { return m_wordmap; }

  /*!
  Read <dataset> element and all the contained <data> elements
  */
  void readData(AOSServices& services, const AXmlElement *pBase);
  
  /*!
  Reset data
  */
  void resetData();

  /*
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  WORDMAP m_wordmap;

  void _loadWords(const AString& type, const AFilename& filename, ALog&);
};

#endif // INCLUDED__ADadaDataHolder_HPP__
