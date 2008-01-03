#ifndef INCLUDED__AEventVisitor_HPP__
#define INCLUDED__AEventVisitor_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ATimer.hpp"

class AException;

#define INVALID_TIME_INTERVAL -1.0

/*!
Event visitor acts as a mini in-memory m_stateTimer
Every time a state is changed, the old state is pushed into an event list
Main purpose is to keep track of all the state changes

Unsynchronized by default
*/
class ABASE_API AEventVisitor : public ADebugDumpable, public AXmlEmittable
{
public:
  /*! 
  ctor
  name - User defined name for this visitor that appears in the emit calls
  */
  AEventVisitor(const AString& name = AConstant::ASTRING_EMPTY);
  
  /*! dtor */
  ~AEventVisitor();

  /*!
  AEmittable
  AXmlEmittable
  */
  void emit(AOutputBuffer&) const;
  void emitXml(AXmlElement&) const;

  /*!
  The name of this event visitor
  Appears in the emit only and is user defined (if not then AEventVisitor is used as name
  */
  AString& useName();

  /*!
  Lifespan timer of this object
  */
  ATimer& useLifespanTimer();

  /*!
  Sets the new state, saving the old
  If isError is true, the error count is incremented
  State time limit is used as a test mark when calling isStateOverTimeLimit() and is used controlled
  */
  void set(const AEmittable& state, bool isError = false, double stateTimeLimit = INVALID_TIME_INTERVAL);
  
  /*!
  Stop the timer
  Current state is saved as the last one
  Stop interval is reset to INVALID_TIME_INTERVAL (not on)
  stopLifespanTimer - if true will also stop the lifespan timer, effectively ending this object's use
  */
  void reset(bool stopLifespanTimer = false);

  /*!
  Clear all events
  Clears timer
  Clears name
  Sets interval to INVALID_TIME_INTERVAL (not on)
  Does not re-enable the visitor (disabled visitors stay disabled and have to be explicitly enabled)
  */
  void clear();

  /*!
  Number of events
  */
  size_t size() const;

  /*!
  Get count for # of errors added
  */
  size_t getErrorCount() const;

  /*!
  Turn event visitor on/off
  */
  void enable(bool);

  /*!
  Access to the state
  State is completely user defined and gives a way to change states and maintain time in state
  */
  const AString &getCurrentState();
  
  /*!
  Milliseconds spent in the current state
  */
  double getTimeIntervalInState() const;

  /*!
  Is the current state ove max time
  */
  bool isStateOverTimeLimit() const;

private:
  //a_Event
  class Event : public ADebugDumpable, public AXmlEmittable
  {
  public:
    Event(const AEmittable& state, double interval, bool isError = false);

    /*!
    AEmittable
    AXmlEmittable
    */
    void emit(AOutputBuffer&) const;
    void emitXml(AXmlElement&) const;

    double m_interval;
    AString m_state;
    bool m_isError;

  public:
  #ifdef __DEBUG_DUMP__
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  #endif
  };
  typedef std::list<Event *> EVENTS;
  EVENTS m_Events;

  //a_Event visitor's name and overall lifespan timer
  AString m_Name;
  ATimer m_LifespanTimer;

  //a_Current event
  Event *mp_CurrentEvent;

  //a_Time spent in state and max time for the state
  ATimer m_stateTimer;
  double m_stateTimeLimit;

  //a_Allows all m_stateTimerging to be disabled
  bool m_isEnabled;

  //a_Set if at least one error occured
  size_t m_errorCount;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AEventVisitor_HPP__
