#ifndef INCLUDED__apiAPythonEmbed_HPP__
#define INCLUDED__apiAPythonEmbed_HPP__

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the APYTHONEMBED_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// APYTHONEMBED_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef APYTHONEMBED_EXPORTS
#  pragma message("APythonEmbed: EXPORT")
#  define APYTHONEMBED_API __declspec(dllexport)
#else
#  pragma message("APythonEmbed: IMPORT")
#  define APYTHONEMBED_API __declspec(dllimport)
#  pragma comment(lib, "APythonEmbed")
#endif

// Importing libraries
#ifndef NDEBUG
#pragma comment(lib, "python24_d")
#else
#pragma comment(lib, "python24")
#endif

#include "Python.h"
#include "apiABase.hpp"

#endif //INCLUDED__apiAPythonEmbed_HPP__