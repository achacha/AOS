#ifndef __pchABase_HPP__
#define __pchABase_HPP__

/*!
ABase external includes
*/
#include "apiABase.hpp"

//! If defined then string allocation will be done via block manager (experiemental for now and not used)
//#define USE_STRING_BLOCK_MANAGER 1

//! non-ANSI helper functions
#include "ansiHelpers.hpp"

//! templates for auto pointers
#include "templateAutoPtr.hpp"

//! Exceptions and asserts
#include "AException.hpp"

#endif //__pchABase_HPP__

