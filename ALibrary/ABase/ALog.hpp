/*
Written by Alex Chachanashvili

$Id$
*/
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
myLog.removeEventMask(ALog::EVENTMASK_ALL_MESSAGES | ALog::EVENTMASK_ALL_WARNINGS);  //remove warnings and messages

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
    EVENT_EXCEPTION        = 0x00000001,       //Exception
    EVENT_CRITICAL_ERROR   = 0x00000002,       //Critical error
    EVENT_FAILURE          = 0x00000004,       //Functional failure
    EVENTMASK_ALL_ERRORS   = 0x000000ff,       //All errors
    
    EVENT_WARNING          = 0x00000100,       //Warning
    EVENTMASK_ALL_WARNINGS = 0x00000f00,       //All warnings
    
    EVENT_MESSAGE          = 0x00001000,       //Log message
    EVENT_STATISTIC        = 0x00002000,       //Statistics of some sort
    EVENT_TIMING           = 0x00004000,       //Timing data
    EVENTMASK_ALL_MESSAGES = 0x000ff000,       //All messages

    EVENT_INFO             = 0x00100000,       //Semi-important information
    EVENT_COMMENT          = 0x00200000,       //Not so important comment
    EVENT_DEBUG            = 0x00400000,       //Debug only, lots of log spam
    EVENTMASK_ALL_INFO     = 0x00f00000,       //All informational

    // 0x0f00000 range of 4 bits currently free

    EVENT_SCOPE_START      = 0x10000000,         //Scope start
    EVENT_SCOPE            = 0x20000000,         //Scope during
    EVENT_SCOPE_END        = 0x40000000,         //Scope end
    EVENT_ALL_SCOPE        = 0xf0000000,         //All scope messages

    EVENTMASK_SILENT       = 0x00000000,         //No messages
    EVENTMASK_DEFAULT      = 0xff0fffff,         //All except INFO logged, INFO must be explicitly turned on
    EVENTMASK_ALL          = 0xffffffff          //Everything
  };

public:
  /*!
  ctor with optional synchronization object to use, NULL implies unsynchronized
  ALog will OWN this synch object and will delete it when done with it
  */
  ALog(ASynchronization *, ALog::EVENT_MASK mask = ALog::EVENTMASK_DEFAULT);
  
  //! virtual dtor
  virtual ~ALog();

  /*!
  Filtering (setting and clearing bits)

  myLog.setEventMask(ALog::EVENTMASK_ALL_ERRORS);  //only log errors
  myLog.addEventMask(ALog::EVENT_TIMING);          //now log errors and TIMING
  myLog.addEventMask(ALog::EVENTMASK_ALL_SCOPE);   //now log erros, TIMING and all scope
  myLog.removeEventMask(ALog::EVENT_SCOPE);        //now log errors, TIMING, SCOPE_START and SCOPE_END

  */
  void setEventMask(u4 mask);             //!< =          (sets a logging mask)
  void addEventMask(u4 mask);             //!< OR         (adds bits specified by mask)
  void removeEventMask(u4 mask);          //!< AND ~mask  (removed bits specified by mask)
  bool isEventMask(u4 mask);              //!< AND        (checks if mask is present)
  u4   getEventMask() const;              //!< Access to the mask

  /*!
  Basic log method for adding events

  Log format:
    YYYYMMDD:HHMMSS{ ... log data ... }

  All AEmittables are appended together, separated by :
  */
  void add(const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);
  void add(const AEmittable&, const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);
  void add(const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);
  void add(const AEmittable&, const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);

  /*!
  Specialized events
  */
  void addException(const AException&);
  void addException(const std::exception&);

  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

  /*!
  AOutputBuffer
  @return Always AConstant::npos since events are written through ALog to AFile periodically
  */
  virtual size_t flush(AFile&);
  
  /*!
  AOutputBuffer
  @return Always AConstant::npos since events are written to AFile periodically
  */
  virtual size_t getSize() const;

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
    void add(const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);
    void add(const AEmittable&, const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);
    void add(const AEmittable&, const AEmittable&, const AEmittable&, u4 eventType = ALog::EVENT_MESSAGE);

    /*!
    AOutputBuffer
    @return Always AConstant::npos since events are written through ALog to AFile periodically
    */
    virtual size_t flush(AFile&);

    /*!
    AOutputBuffer
    @return Always AConstant::npos since events are written through ALog to AFile periodically
    */
    virtual size_t getSize() const;

  protected:
    /*!
    AOutputBuffer
    */
    virtual size_t _append(const char *, size_t);

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
      
      scope.add(ASW("Something happend in this scope",31), ALog::EVENT_WARNING);

      //...do some stuff...

      scope.add(ASW("Execution time=",14), myTimer, ALog::EVENT_TIMING);

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
  virtual size_t _append(const char *, size_t);

  //! Internal synchronization object used when logging
  ASynchronization *mp_SynchObject;

private:
  // No default ctor
  ALog() {}

  u4 m_EventMask;

  static const AString LOG_PREFIX;
  static const AString LOG_SUFFIX;       //a_Includes a trailing CRLF
};

#endif //INCLUDED_ALog_HPP_
