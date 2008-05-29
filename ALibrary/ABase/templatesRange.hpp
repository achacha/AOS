/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__templatesRange_HPP__
#define INCLUDED__templatesRange_HPP__

// clip will return a value that is guaranteed to be between lo and hi
// inclusive.  If the input value 'toClip' is between lo and hi it will be
// returned untouched.  If it is below lo, lo will be returned.  If it is above
// hi, hi will be returned.  The limits lo and hi do not have to be in the
// correct order.
template <class T>
inline T clip(T lo, T toClip, T hi)
{
	if (lo > hi)
		std::swap(lo,hi);
	if (toClip<lo)
		return lo;
	else if (toClip>hi)
		return hi;
	else return toClip;
}

// returns true only if the middle parameter falls inbetween the lo and hi
// parameters.  If it is equal to either lo or hi the function returns false.
// Reguires the less than operator to be defined for type.  The limits lo and
// hi do not have to be in the correct order.
template <class T>
inline int inRange(const T &lo, const T &toCheck, const T &hi)
{
	if (lo > hi)
		return (hi < toCheck) && (toCheck < lo);
	else
		return (lo < toCheck) && (toCheck < hi);
}

// returns true if the middle parameter falls inbetween the lo and hi
// parameters or if it is equal to either lo or hi. Reguires the less than
// operator to be defined for type.  The limits lo and hi do not have to be
// in the correct order.
template <class T>
inline int inClosedRange(const T &lo, const T &toCheck, const T &hi)
{
	if (lo > hi)
		return (hi <= toCheck) && (toCheck <= lo);
	else
		return (lo <= toCheck) && (toCheck <= hi);
}


// returns true only if the middle parameter falls inbetween the lo and hi
// parameters.  If it is equal to either lo or hi the function returns false.
// Reguires the less than operator to be defined for type.  The limits lo and
// hi have to be in the correct order.
template <class T>
inline int inRangeNoSwap(const T &lo, const T &toCheck, const T &hi)
{
	return (lo < toCheck) && (toCheck < hi);
}

// returns true if the middle parameter falls inbetween the lo and hi
// parameters or if it is equal to either lo or hi. Reguires the less than
// operator to be defined for type.  The limits lo and hi have to be in the
// correct order.
template <class T>
inline int inClosedRangeNoSwap(const T &lo, const T &toCheck, const T &hi)
{
	return (lo <= toCheck) && (toCheck <= hi);
}

#endif


