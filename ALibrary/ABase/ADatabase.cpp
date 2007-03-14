#include "pchABase.hpp"
#include "ADatabase.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void ADatabase::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ADatabase @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_urlConnection=" << std::endl;
  m_urlConnection.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "mbool_Initialized=" << (mbool_Initialized ? AString::sstr_True : AString::sstr_False) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

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

void ADatabase::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("ADatabase", 9);

  target.addElement(ASW("url",3), m_urlConnection);
  target.addElement(ASW("isInitialized", 13), mbool_Initialized);
}

const AUrl& ADatabase::getUrl() const
{
  return m_urlConnection;
}
