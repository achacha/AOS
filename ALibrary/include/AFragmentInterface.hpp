#ifndef INCLUDED__AFragment_HPP__
#define INCLUDED__AFragment_HPP__

#include "ADebugDumpable.hpp"
#include "AString.hpp"

#define MAX_PERMUTATIONS 0xffffffff

class AFragmentInterface : public ADebugDumpable
{
public:
	virtual ~AFragmentInterface() {}

	// Back to first permutation
	virtual void reset() = 0;

	// true - there are more permutations in this fragment
	// false - end of the fragment's permutations, reset has been called
	virtual bool next() = 0;
	
	// Permutation maximum of the fragment
	virtual size_t getSetSize() const = 0;
};

#endif //INCLUDED__AFragment_HPP__
