#ifndef INCLUDED_ALog_HPP_
#define INCLUDED_ALog_HPP_

#include "apiABase.hpp"
#include "ASynchronization.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"
#include "ATimer.hpp"

class AException;

/*!
Interface for a lockable log

This is an abstract class. Implementations are ALog_AFile, etc...

Must have some synchronization object, use ASync_Counter in single threaded mode if needed

Example:
ALog myLog(new ASynch_Mutex("MyLogMutex"));                      // everything logged by default
myLog.removeEventMask(ALog::ALL_MESSAGES | ALog::ALL_WARNINGS);  //remove warnings and messages

myLog.add(ASW("Some event",10), ALog::CRITICAL);  // adds a critical event, ASW is an efficient 'const char*' wrapper to 'const AString' (see AString.hpp)
myLog.append(ASW("Another",7));                   // uses AOutputBuffer interface to log a MESSAGE (by default)

...etc...


LOG FORMAT:

{TICK_COUNT}:{EVENT_MASK}{LOG_PREFIX}{LOG_DATA}{LOG_SUFFIX}

Example:

87127763:0000010000{{{Executing module}}}
87127766:0000000002{{{Unable to find file
Filename=foo.txt
Location=bar}}}
...


To derive:

NOTE: Your constructor must pass back ASynchronization* to ALog

-------------------start cut-----------
protected:
  virtual void _add(const AEmittable& event);
-------------------end cut-------------

*/
class ABASE_API ALog : public AXmlEmittable, public AOutputBuffer
{
public:
  /*!
  32 bit mask for event types
  Although there are 32 possible event types, you can combine types (an event can be SCOPE|TIMING|WARNING or ERROR|TIMING, etc...
  The mask is part of the log entry and is useful in log processing/filtering
  Each event is a bit, ALL_* is a mask for a group of events
  */
  enum EVENT_MASK
  {
    EXCEPTION      = 0x00000001,         //Exception
    CRITICAL_ERROR = 0x00000002,         //Critical error
    FAILURE        = 0x00000004,         //Functional failure
    ALL_ERRORS     = 0x000000ff,         //All errors
    
    WARNING        = 0x00000100,         //Warning
    ALL_WARNINGS   = 0x00000f00,         //All warnings
    
    MESSAGE        = 0x00001000,         //Log message
    STATISTIC      = 0x00002000,         //Statistics of some sort
    TIMING         = 0x00004000,         //Timing data
    ALL_MESSAGES   = 0x000ff000,         //All messages

    INFO           = 0x00100000,         //Semi-important information
    COMMENT        = 0x00200000,         //Not so important comment
    DEBUG          = 0x00400000,         //Debug only, lots of log spam
    ALL_INFO       = 0x00f00000,         //All informational

    // 0x0f00000 range of 4 bits currently free

    SCOPE_START    = 0x10000000,         //Scope start
    SCOPE          = 0x20000000,         //Scope during
    SCOPE_END      = 0x40000000,         //Scope end
    ALL_SCOPE      = 0xf0000000,         //All scope messages

    SILENT         = 0x00000000,         //No messages
    DEFAULT        = 0xff0fffff,         //All except INFO logged, INFO must be explicitly turned on
    ALL            = 0xffffffff          //Everything
  };

public:
  /*!
  ALog will OWN this synch object and will delete it when done with it
  */
  ALog(ASynchronization *, ALog::EVENT_MASK mask = ALog::DEFAULT);
  virtual ~ALog();

  /*!
  Filtering (setting and clearing bits)

  myLog.setEventMask(ALog::ALL_ERRORS);       //only log errors
  myLog.addEventMask(ALog::TIMING);           //now log errors and TIMING
  myLog.addEventMask(ALog::ALL_SCOPE);        //now log erros, TIMING and all scope
  myLog.removeEventMask(ALog::SCOPE);         //now log errors, TIMING, SCOPE_START and SCOPE_END

  */
  void setEventMask(u4 mask);             // =          (sets a logging mask)
  void addEventMask(u4 mask);             // OR         (adds bits specified by mask)
  void removeEventMask(u4 mask);          // AND ~mask  (removed bits specified by mask)
  bool isEventMask(u4 mask);              // AND        (checks if mask is present)
  u4   getEventMask() const;              // Access to the mask

  /*!
  Basic log method for adding events

  Log format:
    YYYYMMDD:HHMMSS{ ... log data ... }

  All AEmittables are appended together, separated by :
  */
  void add(const AEmittable&, u4 eventType = ALog::MESSAGE);
  void add(const AEmittable&, const AEmittable&, u4 eventType = ALog::MESSAGE);
  void add(const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::MESSAGE);
  void add(const AEmittable&, const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::MESSAGE);

  /*!
  Specialized events
  */
  void addException(const AException&);
  void addException(const std::exception&);

  /*!
  AXmlEmittable
  */
  void emit(AXmlElement&) const;

public:
  /*!
  Inner class that logs scope only events
  */
  class ABASE_API ScopeEvents : public AOutputBuffer
  {
  public:
    ScopeEvents(const AString&, ALog&);
    ~ScopeEvents();

    /*!
    Events that occured within this scope
    All AEmittables are appended together, separated by :
    */
    void add(const AEmittable&, u4 eventType = ALog::MESSAGE);
    void add(const AEmittable&, const AEmittable&, u4 eventType = ALog::MESSAGE);
    void add(const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::MESSAGE);

  protected:
    virtual void _append(const char *, size_t);  //AOutputBuffer required

  private:
    ALog& m_ParentLog;

    AString m_ScopeName;
    ATimer m_Timer;
  };

  /*!
  Adding a scoped event
  
    MyClass::function()
    {
      // You MUST have a local reference to the returned object or the compiler will optimize it out and
      // you will get an both ctor and dtor at the same time (hence the scope is working and showing how
      // compiler optimized it out but not how it is intended)
      ALog::ScopeEvents& scope = myLog.addScopeEvents("MyClass::function");
      
      //...do some stuff...
      
      scope.add(ASW("Something happend in this scope",31), ALog::WARNING);

      //...do some stuff...

      scope.add(ASW("Execution time=",14), myTimer, ALog::TIMING);

      //Once this class goes out of scope, it will write itself to the log
      //with all additional data (execution time of scope included on last line [milliseconds])
      // for example (format may not be exact):
      // 1223798722:10000000{{{MyClass::function}}}
      // ...
      // 1223798736:20000010{{{MyClass::function:Something happend in this scope}}}
      // ...
      // 1223799321:20004000{{{MyClass::function:Execution time:1536}}}
      // ...
      // 1223799852:40000000{{{MyClass::function:1536}}}
      //
    }
  */
  ALog::ScopeEvents addScopeEvents(const AString&);

protected:
  /*!
  The main log writing method that must be overridden by implementors
  */
  virtual void _add(const AEmittable&, u4 eventType) = 0;

  /*!
  Ability to log messages using AOutputBuffer interface (see AOutputBuffer.hpp)
  */
  virtual void _append(const char *, size_t);

  ASynchronization *mp_SynchObject;

private:
  ALog();  // do default ctor

  u4 m_EventMask;

  static const AString LOG_PREFIX;
  static const AString LOG_SUFFIX;       //a_Includes a trailing CRLF

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ALog_HPP_
