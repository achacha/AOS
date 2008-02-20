#include "pchABase.hpp"
#include "ALog.hpp"
#include "ARope.hpp"
#include "ATime.hpp"

ALog::ScopeEvents::ScopeEvents(const AString& scopeName, ALog& parentLog) :
  m_ParentLog(parentLog),
  m_ScopeName(scopeName),
  m_Timer(true)
{
  m_ParentLog.add(m_ScopeName, ALog::SCOPE_START);
}

ALog::ScopeEvents::~ScopeEvents()
{
  m_Timer.stop();
  m_ParentLog.add(m_ScopeName, m_Timer, ALog::SCOPE_END);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, event_type | ALog::SCOPE);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0,
  const AEmittable& source1,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, source1, event_type | ALog::SCOPE);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0, 
  const AEmittable& source1,
  const AEmittable& source2,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, source2, event_type | ALog::SCOPE);
}

void ALog::ScopeEvents::_append(const char *pcc, size_t len)
{
  AASSERT(this, AConstant::npos != len);
  AASSERT(this, NULL != pcc);
  m_ParentLog.add(m_ScopeName, ASW(pcc, len), ALog::MESSAGE | ALog::SCOPE);
}

size_t ALog::ScopeEvents::flush(AFile& file)
{
  return AConstant::npos;
}

size_t ALog::ScopeEvents::getSize() const
{
  return AConstant::npos;
}
