#ifndef INCLUDED__debugGlobalMemoryMacros_HPP__
#define INCLUDED__debugGlobalMemoryMacros_HPP__

#ifdef _DEBUG

//EXXPERIMENTAL: Enbale trace of allocations of ABase* objects (doesn't always work)
//#  define DEBUG_TRACK_ABASE_MEMORY
#  ifdef DEBUG_TRACK_ABASE_MEMORY 
#  pragma message("Tracing dynamic allocations of ABase* to std::cout")
#  endif

#endif

#endif INCLUDED__debugGlobalMemoryMacros_HPP__
