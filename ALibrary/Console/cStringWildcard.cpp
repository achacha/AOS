#include <iostream>
#include "AString.hpp"
#include "AFragmentString.hpp"
#include "typedefContainers.hpp"

int main()
{
	AFragmentString frag;
	VECTOR_AString vas;
	frag.parse("[123][abc]-foo");
	
	long count = frag.getPermutations();
	for (long i=0; i<count; ++i) {
		AString& str = frag.next();
		vas.push_back(str);
		std::cout << i << ": " << str << std::endl;
	}
}
