#include "pchABase.hpp"
#include "macroDelete.hpp"
#include "ALog_AFile.hpp"
#include "AFile_Physical.hpp"
#include "AException.hpp"
#include "ALock.hpp"
#include "AFileSystem.hpp"
#include "ATime.hpp"
#include "AXmlElement.hpp"
#include "ATextGenerator.hpp"

#define DEFAULT_CYCLE_SLEEP 10000
#define DEFAULT_MAX_FILE_SIZE 1024 * 1024
#define FREESTORE_SIZE 13

void ALog_AFile::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ALog_AFile @ " << std::hex << this << std::dec << ") {" << std::endl;
  ALog::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent+1) << "m_enableLogFileRotate=" << (m_enableLogFileRotate ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_enableSeparateFilesForErrors=" << (m_enableSeparateFilesForErrors ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_filenameRotation=" << std::endl;
  m_filenameRotation.debugDump(os, indent+2);
  
  ADebugDumpable::indent(os, indent+1) << "mp_File=" << std::endl;
  mp_File->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_BufferFreeStore.size()=" << m_BuffersFreeStore.size() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_BufferToWrite.size()=" << m_BuffersToWrite.size() << std::endl;
  BufferContainer::const_iterator cit = m_BuffersToWrite.begin();
  while (cit != m_BuffersToWrite.end())
  {
    ADebugDumpable::indent(os, indent+1) << "{";
    ADebugDumpable::indent(os, indent+2) << ((*cit)->m_ReadyToWrite ? "1" : "0") << std::endl;
    (*cit)->m_Buffer.debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}";
    ++cit;
  }

  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ALog_AFile::ALog_AFile(
  ASynchronization *pSynch,
  const AFilename& baseFilename,
  ALog::EVENT_MASK mask // = ALog::DEFAULT
) :
  ALog(pSynch, mask),
  mp_File(NULL),
  m_CycleSleep(DEFAULT_CYCLE_SLEEP),
  m_LoggerThread(ALog_AFile::threadprocLogger, false),
  m_logMaxFileSize(DEFAULT_MAX_FILE_SIZE),
  m_enableLogFileRotate(true),
  m_DeleteFileObject(true),
  m_enableSeparateFilesForErrors(true)
{
  //a_Base filename
  AFilename f(baseFilename);
  AString newFilename;
  f.emitFilenameNoExt(newFilename);
  
  //a_Timestamp of when the process was started
  ATime timeNow;
  newFilename.append('.');
  timeNow.emitYYYY_MM_DD_HHMMSS(newFilename);
  
  //a_Counter
  newFilename.append(".(6).",5);
  f.emitExtension(newFilename);
  m_filenameRotation.parse(newFilename);

  //a_Find the last file used for logging
  AString filename(1024, 256);
  f.useFilename().clear();
  m_filenameRotation.next(f.useFilename());
  while (AFileSystem::exists(f))
  {
    filename.clear();
    m_filenameRotation.next(f.useFilename());
  }
  
  mp_File = new AFile_Physical(f, ASW("ab+",3));
  
  //a_Initialize free buffers
  m_LoggerThread.setThis(this);
  for (int i=0; i<FREESTORE_SIZE; ++i)
    m_BuffersFreeStore.push_back(new LogBuffer());

  m_LoggerThread.start();
}

ALog_AFile::ALog_AFile(
  ASynchronization *pSynch,
  AFile *pFile,
  ALog::EVENT_MASK mask // = ALog::DEFAULT
) :
  ALog(pSynch, mask),
  mp_File(pFile),
  m_CycleSleep(DEFAULT_CYCLE_SLEEP),
  m_LoggerThread(ALog_AFile::threadprocLogger, false),
  m_logMaxFileSize(AConstant::npos),
  m_enableLogFileRotate(false),
  m_DeleteFileObject(false)
{
  //a_Initialize free buffers
  m_LoggerThread.setThis(this);
  for (int i=0; i<FREESTORE_SIZE; ++i)
    m_BuffersFreeStore.push_back(new LogBuffer());

  m_LoggerThread.start();
}

ALog_AFile::~ALog_AFile()
{
  try
  {
    //a_Stop logger thread
    m_LoggerThread.setRun(false);
    int i=5;
    while(m_LoggerThread.isRunning() && i > 0)
    {
      AThread::sleep(getLoggerCycleSleep());
      --i;
    }
    if (i == 0)
      m_LoggerThread.terminate(-1);

    //a_Flush data
    if (mp_File)
    {
      BufferContainer::iterator it = m_BuffersToWrite.begin();
      if (it != m_BuffersToWrite.end())
      {
        mp_File->open();
        while (it != m_BuffersToWrite.end())
        {
          if (mp_File && (*it)->m_ReadyToWrite)
            mp_File->write((*it)->m_Buffer);
          delete (*it);
          ++it;
        }
        mp_File->close();
      }

      //a_Cleanup
      if (m_DeleteFileObject)
        pDelete(mp_File);
    }

    {
      BufferContainer::iterator it = m_BuffersFreeStore.begin();
      while (it != m_BuffersFreeStore.end())
      {
        delete (*it);
        ++it;
      }
    }
  } 
  catch(...) {}
}

void ALog_AFile::emit(AOutputBuffer& target) const
{
  emitCurrentFilename(target);
}

AXmlElement& ALog_AFile::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  ALog::emitXml(thisRoot);
  thisRoot.addElement(ASW("current_filename",16)).addData(m_filenameRotation, AXmlElement::ENC_CDATADIRECT);
  thisRoot.addElement(ASW("cycle_sleep",11)).addData(AString::fromU4(m_CycleSleep));
  thisRoot.addElement(ASW("file_rotation",13)).addData(m_enableLogFileRotate);
  thisRoot.addElement(ASW("max_file_size",13)).addData(AString::fromU4(m_logMaxFileSize));

  return thisRoot;
}

u4 ALog_AFile::threadprocLogger(AThread& thread)
{
  ALog_AFile *pThis = dynamic_cast<ALog_AFile *>(thread.getThis());
  AASSERT(NULL, pThis);
  AASSERT(NULL, pThis->mp_File);

  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      if (pThis->m_BuffersToWrite.size() > 0)
      {
        pThis->mp_File->open();
      
        while (pThis->m_BuffersToWrite.size() > 0)
        {
          LogBuffer *pBuffer = pThis->m_BuffersToWrite.front();
          if (pBuffer->m_ReadyToWrite)
          {
            ALock lock(pThis->mp_SynchObject);
            pThis->mp_File->write(pBuffer->m_Buffer);
            pThis->m_BuffersToWrite.pop_front();
            pBuffer->clear();
            pThis->m_BuffersFreeStore.push_back(pBuffer);
          }
          else
          {
            //a_Front item is not ready to be written, sleep and come back
            break;
          }
        }

        pThis->mp_File->close();
      }

      if (thread.isRun())
      {
        AThread::sleep(pThis->getLoggerCycleSleep());
        if (pThis->m_enableLogFileRotate && pThis->mp_File)
        {
          //a_Rotate log if size exceeds maximum
          AFile_Physical *pPhysicalFile = dynamic_cast<AFile_Physical *>(pThis->mp_File);
          if (pPhysicalFile && AFileSystem::length(pPhysicalFile->useFilename()) > pThis->m_logMaxFileSize)
          {
            pPhysicalFile->useFilename().useFilename().clear();
            pThis->m_filenameRotation.next(pPhysicalFile->useFilename().useFilename());
          }
        }
      }
    }
    catch(AException& ex)
    {
      //a_TODO: need a better way to report exception
      std::cerr << ex.what() << std::endl;
      thread.setRunning(false);
      return -1;
    }
  }

  thread.setRunning(false);
  return 0;
}

void ALog_AFile::_add(
  const AEmittable& evt,
  u4 eventType
)
{
  LogBuffer *pBuffer = NULL;

  if (
       m_enableSeparateFilesForErrors 
    && (eventType & ALog::ALL_ERRORS)
  )
  {
    //a_Generate semi-random filename
    AString str(1024, 256);
    AFile_Physical *pPhysicalFile = dynamic_cast<AFile_Physical *>(mp_File);
    if (pPhysicalFile)
    {
      pPhysicalFile->useFilename().emit(str);
    }
    else
    {
      m_filenameRotation.emit(str);
    }
    str.append('.');
    ATextGenerator::generateRandomNumber(str, 16);
    str.append(".error.txt",10);
    
    try
    {
      AFile_Physical errorFile(str, "at");
      errorFile.open();
      evt.emit(errorFile);
      errorFile.close();
    }
    catch(...)
    {
      //a_Do nothing, if we are getting exception here we are probably in bad shape as it is 
    }
  }

  //a_Get log buffer
  ALock lock(mp_SynchObject);
  if (m_BuffersFreeStore.size() > 0)
  {
    pBuffer = m_BuffersFreeStore.front();
    m_BuffersFreeStore.pop_front();
  }
  if (!pBuffer)
    pBuffer = new LogBuffer();

  //a_Add new event
  m_BuffersToWrite.push_back(pBuffer);

  //a_Add first, emit later for accurate timing
  evt.emit(pBuffer->m_Buffer);
  pBuffer->m_ReadyToWrite = true;
}

void ALog_AFile::emitCurrentFilename(AOutputBuffer& target) const
{
  if (!mp_File)
    ATHROW(this, AException::InvalidObject);

  AFile_Physical *pPhysicalFile = dynamic_cast<AFile_Physical *>(mp_File);
  if (pPhysicalFile)
    pPhysicalFile->useFilename().emit(target);
}

void ALog_AFile::setLoggerCycleSleep(u4 milliseconds)
{
  AASSERT(this, milliseconds < 60000);  //a_Logger should run at least once an hour
  m_CycleSleep = milliseconds;
}

u4 ALog_AFile::getLoggerCycleSleep() const
{
  return m_CycleSleep;
}

void ALog_AFile::setLoggerMaxFileSize(
  u4 bytes // = AConstant::npos
)
{
  m_logMaxFileSize = bytes;
}

void ALog_AFile::setWriteSeparateErrorFiles(bool b)
{
  m_enableSeparateFilesForErrors = b;
}
