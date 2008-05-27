/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include <iostream>
#include "AEmittable.hpp"
#include "AFile_IOStream.hpp"

//a_Global output operator
std::ostream& operator <<(std::ostream& os, const AEmittable& source)
{
  AFile_IOStream os_file(&os);
  source.emit(os_file);
  return os;
}
