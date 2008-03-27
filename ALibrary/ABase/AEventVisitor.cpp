
#include "pchABase.hpp"
#include "AEventVisitor.hpp"
#include "AFile.hpp"
#include "AXmlElement.hpp"
#include "AXmlData.hpp"
#include "AException.hpp"

void AEventVisitor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AEventVisitor @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) 
    << "m_Name=" << m_Name
    << "  m_LevelThreshold=" << m_LevelThreshold
    << "  m_isEnabled=" << AString::fromBool(m_isEnabled)
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
  EVENTS::const_iterator cit = m_Events.begin();
  while(cit != m_Events.end()) 
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
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
  m_Level(level)
{
  if (m_Visitor.isLogging(m_Level))
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
  if (m_Visitor.isLogging(m_Level))
  {
    ARope message("---dtor[",8);
    message.append(mstr_Where);
    message.append("]",1);
    message.append(mstr_Message);
    m_Visitor.startEvent(message, m_Level);
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
  m_isEnabled(true),
  mp_CurrentEvent(NULL),
  m_stateTimeLimit(INVALID_TIME_INTERVAL),
  m_Name(name),
  m_LevelThreshold(threshold)
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
  delete mp_CurrentEvent;
  for (EVENTS::iterator it = m_Events.begin(); it != m_Events.end(); ++it)
  {
    delete *it;
  }
}

bool AEventVisitor::isLogging(EventLevel level) const
{
  return (level <= m_LevelThreshold && m_isEnabled);
}

void AEventVisitor::startEvent(
  const AEmittable &message, 
  AEventVisitor::EventLevel level,  // = AEventVisitor::EL_EVENT
  double timeLimit             // = INVALID_TIME_INTERVAL
)
{
  //a_If logging is disabled or below threshold, do nothing
  if (level > m_LevelThreshold || !m_isEnabled)
    return;

  if (mp_CurrentEvent)
  {
    //a_Mark current event timer interval
    mp_CurrentEvent->m_interval = m_stateTimer.getInterval();

    //a_Push current event into event list
    m_Events.push_back(mp_CurrentEvent);
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
  //a_If logging is disabled, do nothing
  if (!m_isEnabled)
    return;
  else
  {
    //a_Mark current event timer interval
    if (mp_CurrentEvent)
      mp_CurrentEvent->m_interval = m_stateTimer.getInterval();
  }

  //a_Push current event into event list
  if (mp_CurrentEvent)
    m_Events.push_back(mp_CurrentEvent);

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
  
  if (m_isEnabled)
    target.append(" - Enabled (",12);
  else
    target.append(" - Disabled (",13);

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

  for(EVENTS::const_iterator cit = m_Events.begin(); cit != m_Events.end(); ++cit)
  {
    if ((*cit)->m_level <= threshold)
      (*cit)->emit(target);
  }

  //a_Emit current event
  if (mp_CurrentEvent)
  {
    mp_CurrentEvent->emit(target);
  }

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
  thisRoot.addAttribute(ASW("enabled",7), AString::fromBool(m_isEnabled));
  thisRoot.addElement(ASW("name",4)).addData(m_Name, AXmlElement::ENC_CDATADIRECT);
  thisRoot.addElement(ASW("timer",5)).addData(m_LifespanTimer);

  //a_Emit events
  AXmlElement& events = thisRoot.addElement(ASW("events",6));
  EVENTS::const_iterator cit = m_Events.begin();
  for(EVENTS::const_iterator cit = m_Events.begin(); cit != m_Events.end(); ++cit)
  {
    if ((*cit)->m_level <= threshold)
      (*cit)->emitXml(events.addElement(ASW("event",5)));
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
  return (INVALID_TIME_INTERVAL == m_stateTimeLimit || m_stateTimer.getInterval() < m_stateTimeLimit ? false : true);
}

void AEventVisitor::clear()
{
  m_Events.clear();
  m_ErrorCount = 0;
  m_Name.clear();
  m_LifespanTimer.clear();
  m_LifespanTimer.start();
  m_stateTimer.clear();

  EVENTS::iterator it = m_Events.begin();
  while(it != m_Events.end())
  {
    delete *it;
    ++it;
  }
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

void AEventVisitor::enable(bool b)
{
  m_isEnabled = b;
}

AString& AEventVisitor::useName()
{
  return m_Name;
}

const AString &AEventVisitor::getCurrentEventMessage() const
{
  return (mp_CurrentEvent ? mp_CurrentEvent->m_state : AConstant::ASTRING_NULL);
}

void AEventVisitor::setEventThresholdLevel(AEventVisitor::EventLevel newLevelThreshold)
{
  m_LevelThreshold = newLevelThreshold;
}

AEventVisitor::EventLevel AEventVisitor::getEventThresholdLevel() const
{
  return m_LevelThreshold;
}
