#include "pchABase.hpp"
#include "ACache.hpp"

#ifdef __DEBUG_DUMP__
void ACache::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ACache @ " << std::hex << this << std::dec << ") {" << std::endl;
  AStringHashMap::debugDump(os, indent+1);
//  ADebugDumpable::indent(os, indent+1) << "m_urlConnection=" << std::endl;
//  m_urlConnection.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ACache::ACache(ASynchronization *pSynchObject) :
  AStringHashMap(pSynchObject)
{
}

ACache::~ACache()
{
}
