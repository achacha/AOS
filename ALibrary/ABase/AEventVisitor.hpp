/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AEventVisitor_HPP__
#define INCLUDED__AEventVisitor_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ATimer.hpp"
#include "ABasePtrQueue.hpp"

/*!
Macro for logging events based on level
*/
#define INVALID_TIME_INTERVAL -1.0

/*!
Event visitor acts as a mini in-memory timer
Every time a state is changed, the old state is pushed into an event list
Main purpose is to keep track of all the state changes and how long each state takes

Unsynchronized by design, use sync blocks as needed
*/
class ABASE_API AEventVisitor : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Levels of the event
  */
  enum EventLevel
  {
    EL_NONE     = 0,   //!< Event visitor is disable or ignore event
    EL_ERROR    = 1,   //!< Error (this increments the error counter)
    EL_EVENT    = 2,   //!< Event (DEFAULT) - Event message
    EL_WARN     = 3,   //!< Warning - Warning, non-critical error
    EL_INFO     = 4,   //!< Informational message
    EL_DEBUG    = 5    //!< Debug trace
  };
  
  class ABASE_API ScopedEvent
  {
  public:
    /*!
    Create an event during construction of this object and one during destruction

    @param visitor to use for event logging
    @param whereAt the event is occuring
    @param message optional message
    @param level of the event
    */
    ScopedEvent(
      AEventVisitor& visitor, 
      const AString& whereAt, 
      const AString& message = AConstant::ASTRING_EMPTY, 
      const AEventVisitor::EventLevel level = AEventVisitor::EL_DEBUG);
    
    /*!
    Logs to visitor the destructor message
    */
    ~ScopedEvent();

  private:
    ATimer m_ScopeTimer;
    AEventVisitor& m_Visitor;
    AString mstr_Where;
    AString mstr_Message;
    AEventVisitor::EventLevel m_Level;
  };

public:
  /*! 
  ctor
  
  @param name User defined name for this visitor that appears in the emit calls
  @param threshold Minimum level of the messages to log, default is events and errors
  */
  AEventVisitor(const AString& name = AConstant::ASTRING_EMPTY, AEventVisitor::EventLevel threshold = AEventVisitor::EL_EVENT);
  
  /*! dtor */
  ~AEventVisitor();

  /*!
  AEmittable

  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot to append XML elements to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit based on threshold

  @param target to append to
  @param threshold to use
  */
  void emit(AOutputBuffer& target, AEventVisitor::EventLevel threshold) const;
  
  /*!
  Emit XML based on threshold

  @param thisRoot to append XML elements to
  @param threshold to use
  @return thisRoot for convenience
  */
  AXmlElement& emitXml(AXmlElement& thisRoot, AEventVisitor::EventLevel threshold) const;

  /*!
  The name of this event visitor
  Appears in the emit only and is user defined (if not then AEventVisitor is used as name

  @return reference to the AString object
  */
  AString& useName();

  /*!
  Lifespan timer of this object

  @return reference to the ATimer object
  */
  ATimer& useLifespanTimer();

  /*!
  Check if enabled

  @return if threashold is != EL_NONE
  */
  bool isEnabled() const;

  /*!
  Check if the level specified will be logged by the visitor and it is also enabled

  @param level of the event
  @return if the visitor will log event of this level based on current threshold level and that it is enabled
  */
  bool isLogging(EventLevel level) const;
  
  /*!
  Check if logging level

  @return true if logging level is EL_DEBUG or lower AND enabled
  */
  inline bool isLoggingDebug() const;

  /*!
  Check if logging level

  @return true if logging level is EL_INFO or lower AND enabled
  */
  inline bool isLoggingInfo() const;

  /*!
  Check if logging level

  @return true if logging level is EL_WARN or lower AND enabled
  */
  inline bool isLoggingWarn() const;

  /*!
  Check if logging level

  @return true if logging level is EL_EVENT or lower AND enabled
  */
  inline bool isLoggingEvent() const;

  /*!
  Check if logging level

  @return true if logging level is EL_ERROR or lower AND enabled
  */
  inline bool isLoggingError() const;

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

  @param newLevelThreshold of EventLevel type
  */
  void setEventThresholdLevel(AEventVisitor::EventLevel newLevelThreshold);
  
  /*!
  Get current logging level threshold

  @return EventLevel
  */
  AEventVisitor::EventLevel getEventThresholdLevel() const;

  /*!
  Number of events

  @return count
  */
  size_t size() const;

  /*!
  Get count for # of errors added

  @return error count
  */
  size_t getErrorCount() const;

  /*!
  Access to the current 
  State is completely user defined and gives a way to change states and maintain time in state
  
  @param target to append to
  */
  void getCurrentEventMessage(AOutputBuffer& target) const;
  
  /*!
  Milliseconds spent in the current event state
  
  @return interval of the current event
  */
  double getCurrentEventTimeInterval() const;

  /*!
  Is the current state over max time for the event

  @return true if over limit that user specified
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
    /*!
    Event object ctor
    
    @param state message
    @param level of the event
    @param interval of the event
    */
    Event(const AEmittable& state, AEventVisitor::EventLevel level, double interval);

    //! AEmittable
    virtual void emit(AOutputBuffer& target) const;
    
    //! AXmlEmittable
    virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

    //! Interval
    double m_interval;
    
    //! State message
    AString m_state;
    
    //! Event level
    AEventVisitor::EventLevel m_level;

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  };

  //! Event queue
  ABasePtrQueue m_Events;

  //! Event visitor's name
  AString m_Name;
  
  //! Event visitor's overall lifespan timer
  ATimer m_LifespanTimer;

  //! Current event
  Event *mp_CurrentEvent;

  //! Time spent in state and max time for the state
  ATimer m_stateTimer;
  double m_stateTimeLimit;
  
  //! Event logging threshold
  AEventVisitor::EventLevel m_LevelThreshold;

  //! Set if at least one error occured
  size_t m_ErrorCount;
};

#endif //INCLUDED__AEventVisitor_HPP__
