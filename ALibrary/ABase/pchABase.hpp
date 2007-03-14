#ifndef __pchABase_HPP__
#define __pchABase_HPP__

//a_Includes
// Library linking defines
// osDefines.hpp - OS based flags
// uTypes.hpp    - uX sX and such types
#include "apiABase.hpp"

//a_If defined then string allocation will be done via block manager (experiemental for now and not used)
//#define USE_STRING_BLOCK_MANAGER 1

//a_non-ANSI helper functions
#include "ansiHelpers.hpp"

//a_AutoPointers
#include "templateAutoPtr.hpp"

//a_Exceptions and asserts
#include "AException.hpp"

#endif //__pchABase_HPP__

