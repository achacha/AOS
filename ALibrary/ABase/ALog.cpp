
#include "pchABase.hpp"
#include "ALog.hpp"
#include "ARope.hpp"
#include "ATime.hpp"
#include "ALock.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"

const AString ALog::LOG_PREFIX(":{{{");
const AString ALog::LOG_SUFFIX("}}}\r\n");

#define MESSAGE_SEPARATOR ':'

#ifdef __DEBUG_DUMP__
void ALog::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ALog @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_SynchObject=" << std::hex << (void *)mp_SynchObject << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_EventMask=" << m_EventMask << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ALog::ALog(
  ASynchronization *pSynchObject,
  ALog::EVENT_MASK mask // = ALog::DEFAULT
) :
  mp_SynchObject(pSynchObject),
  m_EventMask(mask)
{
  AASSERT(this, mp_SynchObject);  // can't be NULL, should use ASynch_NULL if no locking is needed
}

ALog::~ALog()
{
  try
  {
    delete mp_SynchObject;
  }
  catch(...) {}
}

void ALog::emitXml(AXmlElement& target) const
{
  AASSERT(this, !target.useName().isEmpty());

  target.addElement(ASW("event_mask",10)).addData(m_EventMask);
}

void ALog::_append(const char *pcc, size_t len)
{
  AASSERT(this, AConstant::npos != len);
  AASSERT(this, NULL != pcc);
  add(ASW(pcc, len), ALog::MESSAGE);
}

void ALog::add(
  const AEmittable& source0, 
  u4 eventType //= ALog::MESSAGE
)
{
  //a_Log event only if event type is not filtered out
  if (m_EventMask & eventType)
  {
    ARope rope;

    //a_timestamp
    ATime timeNow;
    timeNow.emitYYYYMMDDHHMMSS(rope);

    //a_event type
    rope.append(':');
    AString strMask(16, 16);
    strMask.parseU4(eventType, 16);
    strMask.justifyRight(8, '0');
    rope.append(strMask);

    //a_event data
    rope.append(LOG_PREFIX);
    source0.emit(rope);
    rope.append(LOG_SUFFIX);

    //Write to log
    {
      ALock localLock(mp_SynchObject);
      _add(rope, eventType);
    }
  }
}

void ALog::add(
  const AEmittable& source0, 
  const AEmittable& source1, 
  u4 eventType // = ALog::MESSAGE
)
{
  if (m_EventMask & eventType)
  {
    ARope rope;
    
    //a_timestamp in ticks
    ATime timeNow;
    timeNow.emitYYYYMMDDHHMMSS(rope);
    
    //a_event type
    rope.append(':');
    AString strMask(16, 16);
    strMask.parseU4(eventType, 16);
    strMask.justifyRight(8, '0');
    rope.append(strMask);

    //a_event data
    rope.append(LOG_PREFIX);
    source0.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source1.emit(rope);
    rope.append(LOG_SUFFIX);

    {
      ALock localLock(mp_SynchObject);
      _add(rope, eventType);
    }
  }
}

void ALog::add(
  const AEmittable& source0,
  const AEmittable& source1,
  const AEmittable& source2,
  u4 eventType //= ALog::MESSAGE
)
{
  if (m_EventMask & eventType)
  {
    ARope rope;
    ATime timeNow;
    timeNow.emitYYYYMMDDHHMMSS(rope);

    //a_event type
    rope.append(':');
    AString strMask(16, 16);
    strMask.parseU4(eventType, 16);
    strMask.justifyRight(8, '0');
    rope.append(strMask);

    //a_event data
    rope.append(LOG_PREFIX);
    source0.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source1.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source2.emit(rope);
    rope.append(LOG_SUFFIX);

    {
      ALock localLock(mp_SynchObject);
      _add(rope, eventType);
    }
  }
}

void ALog::add(
  const AEmittable& source0,
  const AEmittable& source1,
  const AEmittable& source2,
  const AEmittable& source3,
  u4 eventType //= ALog::MESSAGE
)
{
  if (m_EventMask & eventType)
  {
    ARope rope;
    ATime timeNow;
    timeNow.emitYYYYMMDDHHMMSS(rope);

    //a_event type
    rope.append(':');
    AString strMask(16, 16);
    strMask.parseU4(eventType, 16);
    strMask.justifyRight(8, '0');
    rope.append(strMask);

    //a_event data
    rope.append(LOG_PREFIX);
    source0.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source1.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source2.emit(rope);
    rope.append(MESSAGE_SEPARATOR);
    source3.emit(rope);
    rope.append(LOG_SUFFIX);

    {
      ALock localLock(mp_SynchObject);
      _add(rope, eventType);
    }
  }
}

void ALog::setEventMask(u4 mask)
{
  m_EventMask = mask;
}

void ALog::addEventMask(u4 mask)
{
  m_EventMask |= mask;
}

void ALog::removeEventMask(u4 mask)
{
  m_EventMask &= ~mask;
}

bool ALog::isEventMask(u4 mask)
{
  return (m_EventMask & mask ? true : false);
}

u4 ALog::getEventMask() const
{
  return m_EventMask;
}

ALog::ScopeEvents ALog::addScopeEvents(const AString& scopeName)
{
  return ScopeEvents(scopeName, *this);
}

void ALog::addException(const AException& ex)
{
  const AEmittable& ee = ex;
  add(ee, ALog::EXCEPTION);
}

void ALog::addException(const std::exception& ex)
{
  add(ASWNL(ex.what()), ALog::EXCEPTION);
}
