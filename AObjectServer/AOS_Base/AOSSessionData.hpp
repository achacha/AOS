#ifndef INCLUDED__AOSSessionData_HPP__
#define INCLUDED__AOSSessionData_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AStringHashMap.hpp"
#include "ATimer.hpp"
#include "AXmlEmittable.hpp"

class AOSSessionData : public ADebugDumpable, public AXmlEmittable
{
public:
  AOSSessionData(const AString& sessionId);
  
  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Data is all AString->AString map
  */
  AStringHashMap& useData();
  
  /*!
  Overall age timer of the session
  */
  const ATimer& getAgeTimer() const;
  
  /*!
  Last used timer
  */
  const ATimer& getLastUsedTimer() const;

  /*!
  Timer reset for last time used
  */
  void restartLastUsedTimer();

private:
  AOSSessionData() {}

  AStringHashMap m_Data;
  ATimer m_AgeTimer;
  ATimer m_LastUsedTimer;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSSessionData_HPP__
