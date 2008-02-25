#include "pchABase.hpp"
#include "ADatabase.hpp"
#include "AXmlElement.hpp"

void ADatabase::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ADatabase @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_urlConnection=" << std::endl;
  m_urlConnection.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "mbool_Initialized=" << (mbool_Initialized ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ADatabase::ADatabase() :
  mbool_Initialized(false)
{
}

ADatabase::ADatabase(const AUrl& urlConnection) :
  m_urlConnection(urlConnection),
  mbool_Initialized(false)
{
}

ADatabase::~ADatabase()
{
}

bool ADatabase::isInitialized() const
{
  return mbool_Initialized;
}

AXmlElement& ADatabase::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.addElement(ASW("url",3)).addData(m_urlConnection);
  thisRoot.addElement(ASW("isInitialized", 13)).addData(mbool_Initialized);
}

const AUrl& ADatabase::getUrl() const
{
  return m_urlConnection;
}
