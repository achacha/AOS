
#include "pchAOS_Base.hpp"
#include "AOSUser.hpp"

const AString AOSUser::USERNAME("user");
const AString AOSUser::PASSWORD("pwd");

#ifdef __DEBUG_DUMP__
void AOSUser::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Data={" << std::endl;
  m_Data.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSUser::AOSUser()
{
}

AOSUser::~AOSUser()
{
}

AStringHashMap& AOSUser::useData()
{
  return m_Data;
}

const AStringHashMap& AOSUser::getData() const
{
  return m_Data;
}

void AOSUser::fromXml(const AXmlElement& element)
{
}

void AOSUser::emitXml(AXmlElement& element) const
{
}

void AOSUser::toAFile(AFile& afile) const
{
}

void AOSUser::fromAFile(AFile& afile)
{
}