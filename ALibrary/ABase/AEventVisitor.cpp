/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AEventVisitor.hpp"
#include "AXmlElement.hpp"

void AEventVisitor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) 
    << "m_Name=" << m_Name
    << "  m_LevelThreshold=" << m_LevelThreshold
    << "  m_ErrorCount=" << m_ErrorCount
    << "  m_stateTimeLimit=" << m_stateTimeLimit << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_LifespanTimer=";
  m_LifespanTimer.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "mp_CurrentEvent=";
  if (mp_CurrentEvent)
    mp_CurrentEvent->debugDump(os, indent+1);
  else
    os << "NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_stateTimer=";
  if (mp_CurrentEvent)
    m_stateTimer.debugDump(os, indent+2);
  else
    os << "NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Events={" << std::endl;
  for (const ABase *p = m_Events.getHead(); p; p = p->getNext())
  {
    dynamic_cast<const AEventVisitor::Event *>(p)->debugDump(os, indent+2);
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AEventVisitor::Event::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent)
    << "(AEventVisitor::Event @ " 
    << std::hex << this << std::dec << ") {  "
    << "m_state=" << m_state 
    << "  m_interval=" << m_interval 
    << "  m_level=" << m_level << "  }" << std::endl;
}

AEventVisitor::ScopedEvent::ScopedEvent(
  AEventVisitor& visitor, 
  const AString& strWhere, 
  const AString& strMessage,             //= AConstant::ASTRING_EMPTY
  const AEventVisitor::EventLevel level  // = AEventVisitor::EL_DEBUG
) :
  m_Visitor(visitor),
  mstr_Where(strWhere),
  mstr_Message(strMessage),
  m_Level(level),
  m_ScopeTimer(true)
{
  if (m_Visitor.isLoggingDebug())
  {
    ARope message("+++ctor[",8);
    message.append(mstr_Where);
    message.append("]",1);
    message.append(mstr_Message);
    m_Visitor.startEvent(message, m_Level);
  }
}

AEventVisitor::ScopedEvent::~ScopedEvent()
{
  m_ScopeTimer.stop();
  if (m_Visitor.isLoggingDebug())
  {
    ARope message("---dtor[",8);
    message.append(mstr_Where);
    message.append("]",1);
    message.append(mstr_Message);
    m_Visitor.startEvent(message, m_Level);
  }
  if (m_Visitor.isLogging(m_Level))
  {
    ARope message("---scope(",9);
    message.append(m_ScopeTimer);
    message.append("ms)[",4);
    message.append(mstr_Where);
    message.append(']');
    message.append(mstr_Message);
  }
}

AEventVisitor::AEventVisitor
(
  const AString& name,                 // = AConstant::ASTRING_EMPTY
  AEventVisitor::EventLevel threshold  // = AEventVisitor::EL_EVENT
) :
  m_stateTimer(true),
  m_LifespanTimer(true),
  m_ErrorCount(0),
  mp_CurrentEvent(NULL),
  m_stateTimeLimit(INVALID_TIME_INTERVAL),
  m_LevelThreshold(threshold),
  m_Name(name, 2048, 1024)
{
}

AEventVisitor::Event::Event(
  const AEmittable& state, 
  AEventVisitor::EventLevel level,
  double interval
) :
  m_state(state),
  m_level(level),
  m_interval(interval)
{
}

AEventVisitor::~AEventVisitor()
{
  try
  {
    delete mp_CurrentEvent;
    m_Events.clear(true);
  }
  catch(...) {}
}

bool AEventVisitor::isLogging(EventLevel level) const
{
  return (level <= m_LevelThreshold);
}

bool AEventVisitor::isEnabled() const
{
  return (AEventVisitor::EL_NONE != m_LevelThreshold);
}

bool AEventVisitor::isLoggingDebug() const
{
  return (AEventVisitor::EL_DEBUG <= m_LevelThreshold);
}

bool AEventVisitor::isLoggingInfo() const
{
  return (AEventVisitor::EL_INFO <= m_LevelThreshold);
}

bool AEventVisitor::isLoggingWarn() const
{
  return (AEventVisitor::EL_WARN <= m_LevelThreshold);
}

bool AEventVisitor::isLoggingEvent() const
{
  return (AEventVisitor::EL_EVENT <= m_LevelThreshold);
}

bool AEventVisitor::isLoggingError() const
{
  return (AEventVisitor::EL_ERROR <= m_LevelThreshold);
}

void AEventVisitor::startEvent(
  const AEmittable &message, 
  AEventVisitor::EventLevel level,  // = AEventVisitor::EL_EVENT
  double timeLimit                  // = INVALID_TIME_INTERVAL
)
{
  //a_If logging is disabled or below threshold, do nothing
  if (level > m_LevelThreshold)
    return;

  if (mp_CurrentEvent)
  {
    //a_Mark current event timer interval
    mp_CurrentEvent->m_interval = m_stateTimer.getInterval();

    //a_Push current event into event list
    m_Events.pushBack(mp_CurrentEvent);
  }

  //a_New event
  mp_CurrentEvent = new Event(message, level, 0.0);
  
  if (level <= AEventVisitor::EL_ERROR)
    ++m_ErrorCount;

  //a_Start timer for the current event
  m_stateTimer.start();
  m_stateTimeLimit = timeLimit;
}

void AEventVisitor::startEvent(
  const AEmittable &message, 
  double timeLimit
)
{
  startEvent(message, AEventVisitor::EL_EVENT, timeLimit);
}

void AEventVisitor::endEvent()
{
  //a_Mark current event timer interval and push current event into event list
  if (mp_CurrentEvent)
  {
    mp_CurrentEvent->m_interval = m_stateTimer.getInterval();
    m_Events.pushBack(mp_CurrentEvent);
  }

  //a_New event
  mp_CurrentEvent = NULL;
  m_stateTimeLimit = INVALID_TIME_INTERVAL;
  m_stateTimer.clear();
}

void AEventVisitor::addEvent(
  const AEmittable &message, 
  AEventVisitor::EventLevel level  // = AEventVisitor::EL_EVENT
)
{
  startEvent(message, level, INVALID_TIME_INTERVAL);
  endEvent();
}

void AEventVisitor::emit(AOutputBuffer& target) const
{
  emit(target, m_LevelThreshold);
}

void AEventVisitor::emit(AOutputBuffer& target, AEventVisitor::EventLevel threshold) const
{
  target.append("AEventVisitor {",15);
  target.append(AConstant::ASTRING_EOL);
  if (!m_Name.isEmpty())
    target.append(m_Name);
  
  target.append("(",1);
  m_LifespanTimer.emit(target);
  target.append(") - threshold=",14);
  target.append(AString::fromInt(m_LevelThreshold));

  target.append(" display=",9);
  target.append(AString::fromInt(threshold));

  target.append(" - Errors=",10);
  target.append(AString::fromSize_t(m_ErrorCount));

  target.append(AConstant::ASTRING_EOL);
  target.append('{');
  target.append(AConstant::ASTRING_EOL);

  for (const ABase *p = m_Events.getHead(); p; p = p->getNext())
  {
    const AEventVisitor::Event *pEvent = dynamic_cast<const AEventVisitor::Event *>(p);
    AASSERT(this, pEvent);
    if (pEvent && pEvent->m_level <= threshold)
      pEvent->emit(target);
  }

  //a_Emit current event
  if (mp_CurrentEvent)
    mp_CurrentEvent->emit(target);

  target.append("}}",2);
  target.append(AConstant::ASTRING_EOL);
}

void AEventVisitor::Event::emit(AOutputBuffer& target) const
{
  target.append(' ');
  target.append('0'+(char)m_level);
  target.append(':');

  target.append(AString::fromDouble(m_interval));
  target.append("ms:",3);
  target.append(m_state);
  target.append(AConstant::ASTRING_EOL);
}

AXmlElement& AEventVisitor::emitXml(AXmlElement& thisRoot) const
{
  return emitXml(thisRoot, m_LevelThreshold);
}

AXmlElement& AEventVisitor::emitXml(AXmlElement& thisRoot, AEventVisitor::EventLevel threshold) const
{
  AASSERT(this, !thisRoot.getName().isEmpty());

  thisRoot.addAttribute(ASW("errors",6), AString::fromSize_t(m_ErrorCount));
  thisRoot.addAttribute(ASW("threshold",9), AString::fromSize_t(m_LevelThreshold));
  thisRoot.addAttribute(ASW("display",7), AString::fromSize_t(threshold));
  thisRoot.addElement(ASW("name",4)).addData(m_Name, AXmlElement::ENC_CDATADIRECT);
  thisRoot.addElement(ASW("timer",5)).addData(m_LifespanTimer);

  //a_Emit events
  AXmlElement& events = thisRoot.addElement(ASW("events",6));
  for (const ABase *p = m_Events.getHead(); p; p = p->getNext())
  {
    const AEventVisitor::Event *pEvent = dynamic_cast<const AEventVisitor::Event *>(p);
    AASSERT(this, pEvent);
    if (pEvent && pEvent->m_level <= threshold)
      pEvent->emitXml(events.addElement(ASW("event",5)));
  }
  if (mp_CurrentEvent)
  {
    mp_CurrentEvent->emitXml(events.addElement(ASW("event",5))).addAttribute(ASW("active",6), AConstant::ASTRING_TRUE);
  }
  thisRoot.addElement(ASW("current-state-timer",19)).addData(m_stateTimer);

  return thisRoot;
}

AXmlElement& AEventVisitor::Event::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.getName().isEmpty());

  thisRoot.addAttribute(ASW("interval",8), m_interval);
  thisRoot.addAttribute(ASW("level",5), AString::fromInt(m_level));
  if (m_level <= AEventVisitor::EL_ERROR)
    thisRoot.addAttribute(ASW("error",5), AConstant::ASTRING_TRUE);

  thisRoot.addData(m_state, AXmlElement::ENC_CDATADIRECT);

  return thisRoot;
}

double AEventVisitor::getCurrentEventTimeInterval() const
{
  return m_stateTimer.getInterval();
}

bool AEventVisitor::isCurrentEventOverTimeLimit() const
{
  return (0.0 > m_stateTimeLimit || m_stateTimer.getInterval() < m_stateTimeLimit ? false : true);
}

void AEventVisitor::clear()
{
  m_Events.clear();
  m_ErrorCount = 0;
  m_Name.clear();
  m_LifespanTimer.clear();
  m_LifespanTimer.start();
  m_stateTimer.clear();

  pDelete(mp_CurrentEvent);
}

size_t AEventVisitor::getErrorCount() const
{
  return m_ErrorCount;
}

ATimer& AEventVisitor::useLifespanTimer()
{
  return m_LifespanTimer;
}

size_t AEventVisitor::size() const
{
  return m_Events.size() + (mp_CurrentEvent ? 1 : 0);
}

AString& AEventVisitor::useName()
{
  return m_Name;
}

void AEventVisitor::getCurrentEventMessage(AOutputBuffer& target) const
{
  if (mp_CurrentEvent)
    target.append(mp_CurrentEvent->m_state);
}

void AEventVisitor::setEventThresholdLevel(AEventVisitor::EventLevel newLevelThreshold)
{
  m_LevelThreshold = newLevelThreshold;

  if (AEventVisitor::EL_NONE == m_LevelThreshold && mp_CurrentEvent)
  {
    //a_Disabled, clear last event
    endEvent();
  }
}

AEventVisitor::EventLevel AEventVisitor::getEventThresholdLevel() const
{
  return m_LevelThreshold;
}
