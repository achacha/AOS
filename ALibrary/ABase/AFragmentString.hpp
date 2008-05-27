/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AFragmentString_HPP__
#define INCLUDED__AFragmentString_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

class AFragmentInterface;

class ABASE_API AFragmentString : public ADebugDumpable
{
public:
	typedef std::vector<AFragmentInterface *> VECTOR_Fragment;

public:
	AFragmentString() {}
	AFragmentString(const AString& str);
	~AFragmentString();

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Parse input into fragments
	 if return == str.length() - if all is ok, parsed to end
	 if return < str.length() - position where critical parse error occured
	 parse will always append to what it has, call clear() as needed
  */
	size_t parse(const AString& str);

	/*!
  Reset fragment to initial state
  */
	void reset();

	/*!
  Gets the next permutation
  Returns false if end of permutations is reached (reset is called and it restarts on the next call)
  */
	bool next(AOutputBuffer&);
	
	/*
  Calculates permutations
	AConstant::npos means there were more than MAX_PERMUTATIONS permutations
  */
	size_t getPermutations() const;

	/*!
  Clear contents of the container
  */
	void clear();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
	VECTOR_Fragment m_Container;
	bool m_Finished;
};

#endif //INCLUDED__AFragmentString_HPP__

/*
Evaluation of fragments is right to left.
For {abc} = {a}{b}{c}, subsets are not implicitly combined but treated individually
() counter is limited to 4 byte number (use odometer for bigger)

Fragment	Description
{@}				Uppercase Alpha [A..Z]
{&}				Lowercase Alpha [a..z]
{#}				Number [0..9]
{^}				Uppercase AlphaNum [A..Z0..9]
{%}				Lowercase AlphaNum [a..z0..9]
{?}				AlphaNum [A..Za..z0..9]
(d)				Counter of d digits (2)=[00..99]
(d,m)			Counter of d digits to m (3,5)=[000..005]
(d,m,s)		Counter of d digits from s to m (2,205,90)=[90..205]
[set]			User defined set [set]
<d>       Odometer of d digits
\					Escape character
...everything else is considered a constant as kept as is...


Example:
"{@}(2,90,99)"

Result:
"A90"
..
"A99"
"B90"
..
..
"Z99"


Example:
"{^&^}"

Result:
"AaA"
"AaB"
..
"AaZ"
"AbA"
..
..
"ZzZ"

Example:
"foo-[1c]\\\\(4,3,1)"
(please note \\\\ will be read as \\ after C++ preprocessor is done and we are just escaping a single \ here)

Result:
"foo-1\0001"
"foo-1\0002"
"foo-1\0003"
"foo-c\0001"
"foo-c\0002"
"foo-c\0003"

*/