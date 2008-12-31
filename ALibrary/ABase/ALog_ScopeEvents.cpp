/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ALog.hpp"

ALog::ScopeEvents::ScopeEvents(const AString& scopeName, ALog& parentLog) :
  m_ParentLog(parentLog),
  m_ScopeName(scopeName),
  m_Timer(true)
{
  m_ParentLog.add(m_ScopeName, ALog::EVENT_SCOPE_START);
}

ALog::ScopeEvents::~ScopeEvents()
{
  m_Timer.stop();
  m_ParentLog.add(m_ScopeName, m_Timer, ALog::EVENT_SCOPE_END);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, event_type | ALog::EVENT_SCOPE);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0,
  const AEmittable& source1,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, source1, event_type | ALog::EVENT_SCOPE);
}

void ALog::ScopeEvents::add(
  const AEmittable& source0, 
  const AEmittable& source1,
  const AEmittable& source2,
  u4 event_type //= ALog::MESSAGE
)
{
  m_ParentLog.add(m_ScopeName, source0, source1, source2, event_type | ALog::EVENT_SCOPE);
}

size_t ALog::ScopeEvents::_append(const char *pcc, size_t len)
{
  AASSERT(this, AConstant::npos != len);
  AASSERT(this, NULL != pcc);
  m_ParentLog.add(m_ScopeName, ASW(pcc, len), ALog::EVENT_MESSAGE | ALog::EVENT_SCOPE);
  return len;
}

size_t ALog::ScopeEvents::flush(AFile&)
{
  return AConstant::npos;
}

size_t ALog::ScopeEvents::getSize() const
{
  return AConstant::npos;
}
