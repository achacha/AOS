/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef __debugFileTracer_HPP__
#define __debugFileTracer_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include <fstream>
#include <ctype.h>

/*!
  Description
  ~~~~~~~~~~~
  Intended to be used to trace and debug code to file/debug but can be used under any
  environment if the filename is set correctly.

  Add: 

  Usage (include in your CPP files only)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
\code
    #define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
    #define USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output
    #define AFILE_TRACER_FILENAME "/mypath/myfilename.ext" //OPTIONAL: Use another filename
    #include "debugFileTracer.hpp"

    int MyClass::myMethod()
    {
      AFILE_TRACER_DEBUG_SCOPE("This is a method I am debugging", this);

      <... do some things ...>

      AFILE_TRACER_DEBUG_MESSAGE("This is just a message without ctor/dtor trace", NULL);

      <... do some more things ...>

      return;
    }
\endcode

\verbatim
    Output for this = 0x12345678
    ----------------------------
    +++ctor@0x12345678: This is a method I am debugging
    This is just a message without ctor/dtor trace
    ---dtor@0x12345678: This is a method I am debugging
\endverbatim

    Advanced users may also use: AFILE_TRACER_DEBUG_OBJECT( . . . )
      It creates a memory dump for the object pointer of given length
      in hex and ASCII (printable)

*/

#if defined(__WINDOWS__)
#  define DD_FILENAME "c:\\temp\\debugFileTracer.dump"
#else
#  define DD_FILENAME "/tmp/debugFileTracer.dump"
#endif

//!This is the user specified filename
#ifndef AFILE_TRACER_FILENAME
#  define AFILE_TRACER_FILENAME DD_FILENAME
#endif

#define DD_OPENFLAGS  std::ios::out|std::ios::app|std::ios::binary

/*!
  Message Mode: 0 - Writes in ctor and dtor   1 - Writes only in ctor
  If this class is being redefined, it was incorrectly included in a header file instead of cpp file
*/
class ElRocho
{
  public:
    ElRocho(
      const char * const pccMessage, 
      const void * const pcvAddress,        //a_Address called with
      int iMessageMode,                     //a_If non-zero then dtor will NOT trace
      const char * const pccFilename = AFILE_TRACER_FILENAME
    ) :
      m__pcMessage(NULL),
      m__pcvAddress(pcvAddress),
      m__iMessageMode(iMessageMode),
      m__pcFilename(NULL)
    {
      if (pccMessage)
      {
        size_t length = strlen(pccMessage) + 1;
        m__pcMessage = new char[length];  //a_The message           
#if (_MSC_VER >= 1400)
        strcpy_s(m__pcMessage, length, pccMessage);
#else
        strcpy(m__pcMessage, pccMessage);
#endif
      }

#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
      std::stringstream ofDumpo;
#else
      if (pccFilename)
      {
        size_t length = strlen(pccFilename) + 1;
        m__pcFilename = new char[length];   //a_Filename to use for output
#if (_MSC_VER >= 1400)
        strcpy_s(m__pcFilename, length , pccFilename);
#else
        strcpy(m__pcFilename, pccFilename);
#endif
      }
      std::ofstream ofDumpo((m__pcFilename ? m__pcFilename : DD_FILENAME), DD_OPENFLAGS);
#endif

      if (pcvAddress) ofDumpo << std::hex << pcvAddress << ": ";
      if (iMessageMode == 0) ofDumpo << "+++ctor: ";
			if (pccMessage) ofDumpo << pccMessage << std::endl;

#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
      ofDumpo << std::ends;
      ::OutputDebugStringA(ofDumpo.str().c_str());
#else
      ofDumpo.close();
#endif
    }

    ~ElRocho()
    {
      //a_display dtor message if not only message mode
      if (!m__iMessageMode)
      {                 
#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
        std::stringstream ofDumpo;
#else
        std::ofstream ofDumpo((m__pcFilename ? m__pcFilename : DD_FILENAME), DD_OPENFLAGS);
#endif

        if (m__pcvAddress) ofDumpo << std::hex << m__pcvAddress << ": ";
        ofDumpo << "---dtor:";
        if (m__pcMessage) ofDumpo << m__pcMessage;
			  ofDumpo << std::endl;

#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
        ofDumpo << std::ends;
        ::OutputDebugStringA(ofDumpo.str().c_str());
#else
        ofDumpo.close();
#endif
      }

      delete []m__pcFilename;
			delete []m__pcMessage;
    }

    static void dumpObject(const char *pccMessage, const void *pvObject, u4 u4ObjectSize, const char *pccFilename = AFILE_TRACER_FILENAME)
    {
#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
      std::stringstream ofDumpo;
#else
      std::ofstream ofDumpo((pccFilename ? pccFilename : DD_FILENAME), DD_OPENFLAGS);
#endif

      ofDumpo << "###START: " << pccMessage << " ###" << std::endl;
      ADebugDumpable::dumpMemory_HexAscii(ofDumpo, pvObject, u4ObjectSize);
      ofDumpo << "###END: " << pccMessage << " ###" << std::endl;

#if defined(__WINDOWS__) && defined(USE_WINDOWS_DEBUG_OUTPUT)
      ofDumpo << std::ends;
      ::OutputDebugStringA(ofDumpo.str().c_str());
#else
      ofDumpo.close();
#endif
    }

  private:
    char          *m__pcMessage;
    char          *m__pcFilename;
    int            m__iMessageMode;
    const void    *m__pcvAddress;
};

//a_Undef these macros as they may be enabled/disabled on per file basis
#undef AFILE_TRACER_DEBUG_MESSAGE
#undef AFILE_TRACER_DEBUG_SCOPE
#undef AFILE_TRACER_DEBUG_OBJECT

#ifdef ENABLE_AFILE_TRACER_DEBUG
#  define AFILE_TRACER_DEBUG_MESSAGE(pccMessage, pcvAddress)  { ElRocho tacoPico(pccMessage, pcvAddress, 0x1, AFILE_TRACER_FILENAME); }
#  define AFILE_TRACER_DEBUG_SCOPE(pccMessage, pcvAddress)  ElRocho tacoGrande(pccMessage, pcvAddress, 0x0, AFILE_TRACER_FILENAME)
#  define AFILE_TRACER_DEBUG_OBJECT(pccMessage, pvObject, iObjectSize) ElRocho::dumpObject(pccMessage, pvObject, iObjectSize, AFILE_TRACER_FILENAME)
#else
#  define AFILE_TRACER_DEBUG_MESSAGE(pccMessage, pcvAddress) ((void *)NULL)
#  define AFILE_TRACER_DEBUG_SCOPE(pccMessage, pcvAddress) ((void *)NULL)
#  define AFILE_TRACER_DEBUG_OBJECT(pccMessage, pvObject, iObjectSize) ((void *)NULL)
#endif //ENABLE_AFILE_TRACER_DEBUG

#endif //__debugFileTracer_HPP__
