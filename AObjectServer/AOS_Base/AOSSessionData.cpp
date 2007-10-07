#include "pchAOS_Base.hpp"
#include "AOSSessionData.hpp"

#ifdef __DEBUG_DUMP__
void AOSSessionData::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSSessionData @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Data=" << std::endl;
  m_Data.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_AgeTimer=" << m_AgeTimer.getInterval() << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSSessionData::AOSSessionData(const AString& sessionId) :
  m_Data(NULL, 13),
  m_AgeTimer(true)
{
  m_Data.set(ASW("id",2), sessionId);
}

void AOSSessionData::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("AOSSessionData",14));

  m_AgeTimer.emitXml(target.addElement(ASW("age",3)));
  m_Data.emitXml(target.addElement(ASW("data",4)));
}

double AOSSessionData::getAge()
{
  return m_AgeTimer.getInterval();
}

AStringHashMap& AOSSessionData::useData()
{ 
  return m_Data;
}  

void AOSSessionData::restartTimer()
{
  m_AgeTimer.start();
}

