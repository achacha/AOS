
#include "pchABase.hpp"
#include "AEventVisitor.hpp"
#include "AFile.hpp"
#include "AXmlElement.hpp"
#include "AXmlData.hpp"
#include "AException.hpp"

#ifdef __DEBUG_DUMP__
void AEventVisitor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AEventVisitor @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) 
    << "m_Name=" << m_Name
    << "  m_isEnabled=" << m_isEnabled
    << "  m_errorCount=" << m_errorCount
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
    << "  m_isError=" << m_isError << "  }" << std::endl;
}
#endif

AEventVisitor::AEventVisitor
(
  const AString& name // = AString::sstr_Empty
) :
  m_stateTimer(true),
  m_LifespanTimer(true),
  m_errorCount(0),
  m_isEnabled(true),
  mp_CurrentEvent(NULL),
  m_stateTimeLimit(INVALID_TIME_INTERVAL),
  m_Name(name)
{
}

AEventVisitor::Event::Event(
  const AString& state, 
  double interval,
  bool isError
) :
  m_state(state),
  m_interval(interval),
  m_isError(isError)
{
}

AEventVisitor::~AEventVisitor()
{
  delete mp_CurrentEvent;
  EVENTS::iterator it = m_Events.begin();
  while(it != m_Events.end())
  {
    delete *it;
    ++it;
  }
}

void AEventVisitor::set(const AException& ex)
{
  set(ex.what().toAString(), true);
}

void AEventVisitor::set(
  const AString &state, 
  bool isError,               // = false
  double stateTimeLimit       // = INVALID_TIME_INTERVAL
)
{
  //a_If logging is disabled, do nothing
  if (!m_isEnabled)
    return;

  if (mp_CurrentEvent)
  {
    //a_Mark current event timer interval
    mp_CurrentEvent->m_interval = m_stateTimer.getInterval();

    //a_Push current event into event list
    m_Events.push_back(mp_CurrentEvent);
  }
  //a_New event
  mp_CurrentEvent = new Event(state, 0, isError);
  
  if (isError)
    ++m_errorCount;

  //a_Start timer for the current event
  m_stateTimer.start();
  m_stateTimeLimit = stateTimeLimit;
}

void AEventVisitor::reset(
  bool stopLifespanTimer      // = false
)
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

  //a_Stop lifespan if requested
  if (stopLifespanTimer)
    m_LifespanTimer.stop();
}

void AEventVisitor::emit(AOutputBuffer& target) const
{
  target.append("AEventVisitor {",15);
  target.append(AString::sstr_EOL);
  if (!m_Name.isEmpty())
    target.append(m_Name);
  
  target.append(" (",2);
  m_LifespanTimer.emit(target);
  target.append(')');

  if (m_isEnabled)
    target.append(" - Enabled",10);
  else
    target.append(" - Disabled",11);

  target.append(" - Errors=",10);
  target.append(AString::fromSize_t(m_errorCount));

  target.append(AString::sstr_EOL);
  target.append('{');
  target.append(AString::sstr_EOL);

  EVENTS::const_iterator cit = m_Events.begin();
  while (cit != m_Events.end())
  {
    (*cit)->emit(target);
    ++cit;
  }

  //a_Emit current event
  if (mp_CurrentEvent)
  {
    mp_CurrentEvent->emit(target);
  }

  target.append("}}",2);
  target.append(AString::sstr_EOL);
}

void AEventVisitor::Event::emit(AOutputBuffer& target) const
{
  if (m_isError)
    target.append(" !:",3);
  else
    target.append(" .:",3);

  target.append(AString::fromDouble(m_interval));
  target.append("ms:",3);
  target.append(m_state);
  target.append(AString::sstr_EOL);
}

void AEventVisitor::emit(AXmlElement& target) const
{
  AASSERT(this, !target.getName().isEmpty());

  target.addAttribute(ASW("errors",6), AString::fromSize_t(m_errorCount));

  if (!m_isEnabled)
    target.addAttribute(ASW("enabled",7), AString::sstr_False);

  if (!m_Name.isEmpty())
    target.addElement(ASW("Name",4), m_Name, AXmlData::CDataDirect);

  target.addElement(ASW("Timer",5), m_LifespanTimer);

  //a_Emit events
  AXmlElement& events = target.addElement(ASW("Events",6));
  EVENTS::const_iterator cit = m_Events.begin();
  u8 lastTick = 0;
  u8 firstTick = 0;
  while (cit != m_Events.end())
  {
    (*cit)->emit(events.addElement(ASW("Event",5)));
    ++cit;
  }
  if (mp_CurrentEvent)
  {
    AXmlElement& e = events.addElement(ASW("Event",5));
    e.addAttribute(ASW("active",6), AString::sstr_True);
    mp_CurrentEvent->emit(e);
  }
  target.addElement(ASW("CurrentStateTimer",17), m_stateTimer);
}

void AEventVisitor::Event::emit(AXmlElement& target) const
{
  AASSERT(this, !target.getName().isEmpty());

  target.addAttribute(ASW("interval",8), m_interval);
  if (m_isError)
    target.addAttribute(ASW("error",5), AString::sstr_True);
  target.addData(m_state, AXmlData::CDataDirect);
}

double AEventVisitor::getTimeIntervalInState() const
{
  return m_stateTimer.getInterval();
}

bool AEventVisitor::isStateOverTimeLimit() const
{
  return (INVALID_TIME_INTERVAL == m_stateTimeLimit || m_stateTimer.getInterval() < m_stateTimeLimit ? false : true);
}

void AEventVisitor::clear()
{
  m_Events.clear();
  m_errorCount = 0;
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
  return m_errorCount;
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

const AString &AEventVisitor::getCurrentState()
{
  return (mp_CurrentEvent ? mp_CurrentEvent->m_state : AString::sstr_Null);
}
