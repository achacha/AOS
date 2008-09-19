/*
Written by Alex Chachanashvili

$Id$
*/

// Remove 4xxx warning that are part of 3rd party code (not going to stop warnings when compiling their source files)
#if _MSC_VER > 1000
  #pragma warning(disable:4244)  // conversion from 'foo' to 'bar', possible loss of data
  #pragma warning(disable:4267)  // conversion from 'foo' to 'bar', possible loss of data
  #pragma warning(disable:4306)  // conversion from 'foo' to 'bar' of greater size
  #pragma warning(disable:4324)  // structure was padded due to __declspec(align())
  #pragma warning(disable:4701)  // potentially uninitialized local variable 'foo' used
  #pragma warning(disable:4996)  // This function or variable may be unsafe. Consider using XXXX_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif

#include "apiABase.hpp"
#include "apiAGdLib.hpp"
#include "png.h"
