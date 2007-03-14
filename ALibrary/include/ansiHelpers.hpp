#ifndef __ansiHELPERS_HPP__
#define __ansiHELPERS_HPP__

//a_This is defines in osDefines.hpp and is compiler/OS specific
#ifdef NEEDS_ANSI_HELPERS

//a_Get the functions and add only what is needed
#include <stdlib>
#include <string>

#ifndef strrev
  char *strrev (char *pcString);
#endif

#ifndef strnicmp
  int strnicmp(const char *pccOne, const char *pccTwo, u4 uLength);
#endif

#ifndef xtoa
  void xtoa (u4 uNumber, char *pcBuffer, u4 uRadix, int iIsNegative);
#endif

#ifndef itoa
  char *itoa (int iNumber, char *pcBuffer, u4 uRadix);
#endif

#ifndef ltoa
  char *ltoa (long lNumber, char *pcBuffer, u4 uRadix);
#endif

#ifndef ultoa
  char *ultoa (u4 uNumber, char *pcBuffer, u4 uRadix);
#endif

#endif
#endif  //a_ __ansiHELPERS_HPP__

