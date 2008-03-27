#ifndef INCLUDED__AEventVisitor_HPP__
#define INCLUDED__AEventVisitor_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ATimer.hpp"

class AException;
class AEventVisitor;

/*!
Macro for logging events based on level
*/
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
  Levels of the event
  */
  enum EventLevel
  {
    EL_ERROR  = 1,   // Error (this increments the error counter)
    EL_EVENT  = 2,   // Event (DEFAULT) - Event message
    EL_WARN   = 3,   // Warning - Warning, non-critical error
    EL_INFO   = 4,   // Informational message
    EL_DEBUG  = 5    // Debug trace
  };
  
  class ABASE_API ScopedEvent
  {
  public:
    /*!
    Create an event during construction of this object and one during destruction

    @param visitor to use for event logging
    @param where the event is occuring
    @param message optional message
    */
    ScopedEvent(
      AEventVisitor& visitor, 
      const AString& strWhere, 
      const AString& strMessage = AConstant::ASTRING_EMPTY, 
      const AEventVisitor::EventLevel level = AEventVisitor::EL_DEBUG);
    
    /*!
    Logs to visitor the destructor message
    */
    ~ScopedEvent();

  private:
    AEventVisitor& m_Visitor;
    AString mstr_Where;
    AString mstr_Message;
    AEventVisitor::EventLevel m_Level;
  };

public:
  /*! 
  ctor
  
  @param name  - User defined name for this visitor that appears in the emit calls
  @param level - Minimum level of the messages to log, default is events and errors
  */
  AEventVisitor(const AString& name = AConstant::ASTRING_EMPTY, AEventVisitor::EventLevel threshold = AEventVisitor::EL_EVENT);
  
  /*! dtor */
  ~AEventVisitor();

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit based on threshold
  */
  void emit(AOutputBuffer& target, AEventVisitor::EventLevel threshold) const;
  AXmlElement& emitXml(AXmlElement& thisRoot, AEventVisitor::EventLevel threshold) const;

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
  Check if the level specified will be logged by the visitor and it is also enabled

  @param level of the event
  @return if the visitor will log event of this level based on current threshold level and that it is enabled
  */
  bool isLogging(EventLevel level) const;
  
  /*!
  Sets the new state, stopping and saving current one
  State time limit is used as a test mark when calling isStateOverTimeLimit() and is user controlled

  @param message - Message of the event
  @param level - EventLevel of this event
  @param timeLimit - How long should this state be active, when over the limit isStateOverTimeLimit returns true
  */
  void startEvent(const AEmittable& message, EventLevel level = AEventVisitor::EL_EVENT, double timeLimit = INVALID_TIME_INTERVAL);
  
  /*!
  Sets new event and time limit (defaults level to EL_EVENT)
  
  @param message - Message of the event
  @param timeLimit - How long should this state be active, when over the limit isStateOverTimeLimit returns true
  */
  void startEvent(const AEmittable& message, double timeLimit);

  /*!
  End the current event and associated timer, current event is saved as the last one
  Stop interval is reset to INVALID_TIME_INTERVAL (not on)
  Does not stop lifespan timer (which is associated with the entire event visitor)
  */
  void endEvent();

  /*!
  Start and stop event effectively adding it
  The currect event is saved, then this new event is started and ended
  There is no current event after this call

  @param message - Message of the event
  @param level - EventLevel of this event
  */
  void addEvent(const AEmittable& message, EventLevel level = AEventVisitor::EL_EVENT);

  /*!
  Clear the event visitor
  Clear all events
  Clear all timers
  Clear name
  Sets interval to INVALID_TIME_INTERVAL (not on)
  Does not re-enable the visitor (disabled visitors stay disabled and have to be explicitly enabled)
  */
  void clear();

  /*!
  Sets new logging level threshold (does not retroactively remove old events below the current threshold)
  */
  void setEventThresholdLevel(AEventVisitor::EventLevel newLevelThreshold);
  
  /*!
  Get current logging level threshold
  */
  AEventVisitor::EventLevel getEventThresholdLevel() const;

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
  Access to the current 
  State is completely user defined and gives a way to change states and maintain time in state
  @return Current message or AConstant::
  */
  const AString &getCurrentEventMessage() const;
  
  /*!
  Milliseconds spent in the current event state
  */
  double getCurrentEventTimeInterval() const;

  /*!
  Is the current state over max time for the event
  */
  bool isCurrentEventOverTimeLimit() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Event
  class Event : public ADebugDumpable, public AXmlEmittable
  {
  public:
    Event(const AEmittable& state, AEventVisitor::EventLevel level, double interval);

    /*!
    AEmittable
    AXmlEmittable
    */
    virtual void emit(AOutputBuffer& target) const;
    virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

    double m_interval;
    AString m_state;
    AEventVisitor::EventLevel m_level;

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
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

  //a_Allows all event logging to be disabled
  bool m_isEnabled;
  
  //a_Event logging threshold
  AEventVisitor::EventLevel m_LevelThreshold;

  //a_Set if at least one error occured
  size_t m_ErrorCount;
};

#endif //INCLUDED__AEventVisitor_HPP__
