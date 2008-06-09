/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAGdLib.hpp"
#include "AGdLibrary.hpp"
#include "AException.hpp"

AGdLibrary::AGdLibrary()
{
  AVERIFY(NULL, !gdFontCacheSetup());
}

AGdLibrary::~AGdLibrary()
{
  gdFontCacheShutdown();
}
