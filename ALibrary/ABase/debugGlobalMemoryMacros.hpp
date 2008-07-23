#ifndef INCLUDED__debugGlobalMemoryMacros_HPP__
#define INCLUDED__debugGlobalMemoryMacros_HPP__

#ifdef _DEBUG

// Optional to enbale std::cout trace of allocations of ABase* objects
#  define DEBUG_TRACK_ABASE_MEMORY
#  ifdef DEBUG_TRACK_ABASE_MEMORY 

//a_Only uncomment if you want every allocation/deallocation traced to std::cout
//#  define DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT

#  pragma message("Tracing dynamic allocations of ABase* to std::cout")
#  endif

#endif

#endif INCLUDED__debugGlobalMemoryMacros_HPP__
