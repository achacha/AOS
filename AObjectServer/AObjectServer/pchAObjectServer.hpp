#ifndef __preAObjectServer_HPP__
#define __preAObjectServer_HPP__

#define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
#ifdef __DEBUG_DUMP__
#define USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output
#endif
#include <debugFileTracer.hpp>
#define AOS_DEBUGTRACE(msg, ptr) do { \
  AFILE_TRACER_DEBUG_MESSAGE(msg, ptr); \
  std::cout << msg << std::endl; \
} while(0)

#include <apiAOS_Base.hpp>

#include <AString.hpp>
#include <ANumber.hpp>
#include <AException.hpp>
#include <templateAutoPtr.hpp>
#include <AAttributes.hpp>
#include <AObjectContainer.hpp>
#include <ALog.hpp>
#include <ATextConverter.hpp>
#include <templateSingletonPattern.hpp>
#include <AXmlElement.hpp>
#include <AINIProfile.hpp>
#include <AFile.hpp>
#include <ARope.hpp>
#include <ALock.hpp>
#include <AFile_AString.hpp>

#include <templateAutoPtr.hpp>
#include <templateListOfPtrs.hpp>

#endif //__preAObjectServer_HPP__
