/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ALog_AFile_HPP__
#define INCLUDED__ALog_AFile_HPP__

#include "apiABase.hpp"
#include "templateAutoPtr.hpp"
#include "AString.hpp"
#include "ALog.hpp"
#include "ASynchronization.hpp"
#include "AFile.hpp"
#include "AFilename.hpp"
#include "AThread.hpp"
#include "ATime.hpp"
#include "AFragmentString.hpp"

// ALog_AFile used for file based logging
class AException;
class ASync_Mutex;

class ABASE_API ALog_AFile : public ALog
{
public:
  //! Default sleep cycle
  static const u4 DEFAULT_CYCLE_SLEEP;
  
  //! Default max file size
  static const u4 DEFAULT_MAX_FILE_SIZE;

public:
  /*!
  Filename specific to this log and used as a base name in log rotation (BASENAME.EXT -> BASENAME.YYYYMMDD.NNNNNN.EXT)
  The date on the log file will be that of the time the log object was created (not the date of the actual event, that info is inside the log)

  Usage:
    ALog_AFile mylog(new ASync_CriticalSection(), "mydir/mylogname.log");
    // e.g. log file may be mydir/mylogname.20060522.000000.log, then after rotate mydir/mylogname.20060522.000001.log
    //...etc...

  
  ASynchronization object will be OWNED by this class and DELETED when done
  */
  ALog_AFile(ASynchronization *, const AFilename&, ALog::EVENT_MASK mask = ALog::EVENTMASK_DEFAULT);
  
  /*!
  Log to AFile object (no file rotation is done)
  NOTE: This object will OWN the AFile * and will delete it when it is done

  Usage:
    AAutoPtr<AFile> pFile(new AFile_AString());
    ALog_AFile mylog(new ASync_CriticalSection(), pFile.get());
    //...etc...


  ASynchronization object will be OWNED by this class and DELETED when done
  AFile object will be NOT BE OWNED, you must delete it
  */
  ALog_AFile::ALog_AFile(ASynchronization *pSynch, AFile *pFile, ALog::EVENT_MASK mask = ALog::EVENTMASK_DEFAULT);
  
  // dtor
  virtual ~ALog_AFile();

  /*!
  Access to filename
  Base is used in generating current in log rotation (daily and when over max file size)
  e.g.  Foo.log -> Foo.20060512.000000.log, Foo.20060512.000001.log, Foo.20060513.000000.log, etc...
  */
  void emitCurrentFilename(AOutputBuffer&) const;
  
  /*!
  If true will also write a separate semi-randomly named file for every error that occurs
  */
  void setWriteSeparateErrorFiles(bool b = true);

  /*!
  Logger thread operational parameters
  */
  void setLoggerCycleSleep(u4 milliseconds);
  u4 getLoggerCycleSleep() const;
  
  /*!
  Maximum file size per log file
  */
  void setLoggerMaxFileSize(u4 bytes = ALog_AFile::DEFAULT_MAX_FILE_SIZE);

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  ALog
  */
  virtual void _add(const AEmittable&, u4 eventType);

private:
  // Worker thread that writes the data
  AThread m_LoggerThread;
  static u4 threadprocLogger(AThread&);
  u4 m_CycleSleep;
  
  // Special error handling
  bool m_enableSeparateFilesForErrors;

  // Log rotation
  bool m_enableLogFileRotate;
  u4 m_logMaxFileSize;
  AFragmentString m_filenameRotation;

  // Data buffer for the log
  class LogBuffer
  {
  public:
    LogBuffer() : m_ReadyToWrite(false) {}
    void clear() { m_ReadyToWrite = false; m_Buffer.clear(); }
    ARope m_Buffer;
    bool m_ReadyToWrite;
  };
  typedef std::list<LogBuffer *> BufferContainer;
  BufferContainer m_BuffersToWrite;

  // Pointer to the AFile based object and filename of this log
  bool m_DeleteFileObject;
  AFile *mp_File;
};

#endif //INCLUDED__ALog_AFile_HPP__
