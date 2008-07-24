/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSSessionData.hpp"

const AString AOSSessionData::SESSIONID("id",2);

void AOSSessionData::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Data=" << std::endl;
  m_Data.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_AgeTimer=" << m_AgeTimer.getInterval() << " ms" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_LastUsedTimer=" << m_AgeTimer.getInterval() << " ms" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSSessionData::AOSSessionData(const AString& sessionId) :
  m_AgeTimer(true),
  m_LastUsedTimer(true),
  m_Data(ASW("data",4))
{
  m_Data.useRoot().setString(SESSIONID, sessionId);
}

AOSSessionData::AOSSessionData(AFile& aFile) :
  m_AgeTimer(true),
  m_LastUsedTimer(true),
  m_Data(ASW("data",4))
{
  fromAFile(aFile);
  AASSERT_EX(this, m_Data.useRoot().exists(SESSIONID), ASWNL("Session ID was not found after session was restored"));
}

bool AOSSessionData::getSessionId(AOutputBuffer& target) const
{
  const AXmlElement *p = m_Data.getRoot().findElement(SESSIONID);
  if (p)
  {
    p->emitContent(target);
    return true;
  }
  else
    return false;
}

AXmlElement& AOSSessionData::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  m_AgeTimer.emitXml(thisRoot.addElement(ASW("session-age",11)));
  m_LastUsedTimer.emitXml(thisRoot.addElement(ASW("last-used",9)));
  m_Data.getRoot().emitXml(thisRoot);

  return thisRoot;
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

AXmlElement& AOSSessionData::useData()
{ 
  return m_Data.useRoot();
}

const AXmlElement& AOSSessionData::getData() const
{ 
  return m_Data.getRoot();
}

void AOSSessionData::toAFile(AFile& aFile) const
{
  m_Data.toAFile(aFile);
}

void AOSSessionData::fromAFile(AFile& aFile)
{
  m_Data.fromAFile(aFile);
}

void AOSSessionData::clear()
{
  m_AgeTimer.clear();
  m_LastUsedTimer.clear();
  m_Data.clear();
}
