/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiAPythonEmbed_HPP__
#define INCLUDED__apiAPythonEmbed_HPP__

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