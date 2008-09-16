/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef __ATimer_HPP__
#define __ATimer_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ABASE_API ATimer : public ADebugDumpable, public AXmlEmittable
{          
public:
  /*!
  Create new timer (by default not started)

  @param startNow if true will start timer upon creation
  */
  ATimer(bool startNow = false);
  
  //! dtor
  ~ATimer();

  /*!
  AEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot XML element that represents this class to append to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Resets(clears) the timer and starts it again
  */
  void start();
  
  /*!
  Stops timer
  */
  void stop();

  /*!
  Clears timer and sets to unstarted state
  */
  void clear();

  /*!
  Checks timer running state

  @return true if timer is running
  */
  bool isRunning() const;

  /*!
  Gets milliseconds between start time and either stop time or if still running current time
  Will not stop timer if still runnning

  @return interval in milliseconds
  */
  double getInterval(bool boolStopTimer = false);

  /*!
  Gets milliseconds between start time and either stop time or if still running current time
  Will not stop timer if still runnning

  @return interval in milliseconds
  */
  double getInterval() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  /*!
  High performance counter frequency (# of counts per second)
  Machine and platform specific
  */
  static double sm_frequency;

  u8 m_startCount;
  u8 m_stopCount;
  
  bool mbool_Running;
};

#endif
