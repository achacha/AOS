#include "pchAOS_Base.hpp"
#include "AOSSessionData.hpp"

#ifdef __DEBUG_DUMP__
void AOSSessionData::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSSessionData @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Data=" << std::endl;
  m_Data.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_AgeTimer=" << m_AgeTimer.getInterval() << " ms" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_LastUsedTimer=" << m_AgeTimer.getInterval() << " ms" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSSessionData::AOSSessionData(const AString& sessionId) :
  m_Data(NULL, 13),
  m_AgeTimer(true),
  m_LastUsedTimer(true)
{
  m_Data.set(ASW("id",2), sessionId);
}

void AOSSessionData::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("AOSSessionData",14));

  m_AgeTimer.emitXml(target.addElement(ASW("session-age",11)));
  m_LastUsedTimer.emitXml(target.addElement(ASW("last-used",9)));
  m_Data.emitXml(target.addElement(ASW("data",4)));
}

const ATimer& AOSSessionData::getAgeTimer() const
{
  return m_AgeTimer;
}

const ATimer& AOSSessionData::getLastUsedTimer() const
{
  return m_LastUsedTimer;
}

void AOSSessionData::restartLastUsedTimer()
{
  m_LastUsedTimer.start();
}

AStringHashMap& AOSSessionData::useData()
{ 
  return m_Data;
}
