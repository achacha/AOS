#include "pchABase.hpp"


#include "ATimer.hpp"
#include "ATime.hpp"
#include "AXmlElement.hpp"

double ATimer::sm_frequency = 0.001f * ATime::getHighPerfFrequency();

#ifdef __DEBUG_DUMP__
void ATimer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) 
    << "(ATimer @ " << std::hex << this << std::dec << ") {  "
    << "  getInterval()=" << getInterval()
    << "  mbool_Running=" << (mbool_Running ? 1 : 0)
    << "  m_startCount=" << m_startCount
    << "  m_stopCount=" << m_stopCount
    << "  sm_frequency=" << sm_frequency
    << "  }" << std::endl;
}
#endif

ATimer::ATimer(bool boolStartNow) :
  m_startCount(0),
  m_stopCount(0),
  mbool_Running(false)
{
  if (boolStartNow)
    start();
}

ATimer::~ATimer()
{
}

void ATimer::clear()
{
  m_startCount = 0;
  m_stopCount  = 0;
  mbool_Running = false;
}

void ATimer::start()
{
  mbool_Running = true;
  m_startCount = ATime::getHighPerfCounter();
  m_stopCount = 0;
}

void ATimer::stop()
{
  if (m_startCount)
    m_stopCount = ATime::getHighPerfCounter();
  else
    m_stopCount = 0;  //a_Never started

  mbool_Running = false;
}

double ATimer::getInterval(bool boolStopTimer)
{ 
  if (boolStopTimer)
  {
    stop();

    double diff = (double)m_stopCount;
    diff -= m_startCount;
    diff /= sm_frequency;
    return diff;
  }
  
  if (mbool_Running) 
  {
    if (m_startCount)
    {
      double diff = (double)ATime::getHighPerfCounter();
      diff -= m_startCount;
      diff /= sm_frequency;
      return diff;
    }
    else
      return 0.0; //a_Timer never started
  }
  else
  {
    double diff = (double)m_stopCount;
    diff -= m_startCount;
    diff /= sm_frequency;
    return diff;
  }
}

double ATimer::getInterval() const
{
  if (mbool_Running)
  {
    if (m_startCount)
    {
      double diff = (double)ATime::getHighPerfCounter();
      diff -= m_startCount;
      diff /= sm_frequency;
      return diff;
    }
    else
      return 0.0; //a_Timer never started
  }
  else
  {
    double diff = (double)m_stopCount;
    diff -= m_startCount;
    diff /= sm_frequency;
    return diff;
  }
}

void ATimer::emit(AOutputBuffer& target) const
{
  target.append(AString::fromDouble(getInterval()));
  target.append("ms",2);
}

void ATimer::emit(AXmlElement& target) const
{
  AASSERT(this, !target.useName().isEmpty());

  if (mbool_Running)
    target.addAttribute(ASW("running",7), AString::sstr_True);

  target.addElement(ASW("interval",8), AString::fromDouble(getInterval()))
    .addAttribute(ASW("freq", 4), AString::fromDouble(sm_frequency, 6))
    .addAttribute(ASW("unit", 4), ASW("ms",2));

  target.addElement(ASW("start_count",11), AString::fromU8(m_startCount));
  target.addElement(ASW("stop_count",10), AString::fromU8(m_stopCount));
}

bool ATimer::isRunning() const
{
  return mbool_Running;
}
