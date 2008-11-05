/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSSessionData.hpp"
#include "ASync_CriticalSection.hpp"
#include "AOSContext.hpp"

const AString AOSSessionData::SESSIONID("id",2);
const AString AOSSessionData::LOCALE("locale",6);
const AString AOSSessionData::ROOT("data",4);

void AOSSessionData::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Data=" << std::endl;
  m_Data.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) 
    << "m_AgeTimer=" << m_AgeTimer.getInterval() << " ms"
    << "  m_LastUsedTimer=" << m_AgeTimer.getInterval() << " ms" << std::endl;

  ADebugDumpable::indent(os, indent+1) 
    << "m_InUseCount=" << m_InUseCount
    << "  mp_SyncObject=" << AString::fromPointer(mp_SyncObject) << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSSessionData::AOSSessionData(const AString& sessionId) :
  m_AgeTimer(true),
  m_LastUsedTimer(true),
  m_Data(ROOT),
  m_InUseCount(0),
  mp_SyncObject(NULL)
{
  m_Data.useRoot().setString(SESSIONID, sessionId);
}

AOSSessionData::AOSSessionData(AFile& aFile) :
  m_AgeTimer(true),
  m_LastUsedTimer(true),
  m_Data(ROOT),
  m_InUseCount(0),
  mp_SyncObject(NULL)
{
  fromAFile(aFile);
  AASSERT_EX(this, m_Data.useRoot().exists(SESSIONID), ASWNL("Session ID was not found after session was restored"));
}

AOSSessionData::~AOSSessionData()
{
  AASSERT(this, 0 == m_InUseCount);  //a_Uneven number of calls to init/finalize
  AASSERT(this, !mp_SyncObject);     //a_Uneven number of calls to init/finalize
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

ASynchronization *AOSSessionData::useSyncObject()
{
  return mp_SyncObject;
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

void AOSSessionData::init(AOSContext& context)
{
  if (!m_InUseCount)
  {
    mp_SyncObject = new ASync_CriticalSection();
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
    {
      context.useEventVisitor().startEvent(ASW("AOSSessionData::init: Allocated session data sync object",56), AEventVisitor::EL_DEBUG);
    }
  }
  ++m_InUseCount;
  AASSERT(this, mp_SyncObject);  //If we are here and it asserts the calling code is not synchronizing correctly
}

void AOSSessionData::finalize(AOSContext& context)
{
  m_InUseCount = 0;
  pDelete(mp_SyncObject);
  
  if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    context.useEventVisitor().startEvent(ASW("AOSSessionData::finalize: Deallocated session data sync object",62), AEventVisitor::EL_DEBUG);
  }
}
