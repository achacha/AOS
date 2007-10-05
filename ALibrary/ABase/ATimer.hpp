#ifndef __ATimer_HPP__
#define __ATimer_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ABASE_API ATimer : public ADebugDumpable, public AXmlEmittable
{          
public:
  //a_If true, the timer will start upon creation
  ATimer(bool boolStartNow = false);
  ~ATimer();

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  //a_Timer control
  void start();           //a_Resets(clears) the timer and starts it again
  void stop();            //a_Stops timer
  void clear();           //a_Clears timer, sets to unstarted state
  bool isRunning() const; //a_true if timer is running

  //a_Get difference in milliseconds
  double getInterval(bool boolStopTimer = false);
  double getInterval() const;                       //a_Does not stop timer

private:
  /*!
  High performance counter frequency (# of counts per second)
  Machine and platform specific
  */
  static double sm_frequency;

  u8 m_startCount;
  u8 m_stopCount;
  
  bool mbool_Running;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif
