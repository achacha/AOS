#ifndef INCLUDED__extABase_HPP__
#define INCLUDED__extABase_HPP__

//a_STD library
#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <set>

//a_Global helper operators
class AEmittable;

ABASE_API std::ostream& operator <<(std::ostream& os, const AEmittable& str);

#endif
