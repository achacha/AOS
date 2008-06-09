/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AGdLibrary_HPP__
#define INCLUDED__AGdLibrary_HPP__

#include "apiAGdLib.hpp"

/*!
Initialize gd library
  Setup/teardown font cache

Currently not required unless you plan on using gd functions that use fonts.
*/
class AGDLIB_API AGdLibrary
{
public:
  /*!
  Initialize gd library
  */
  AGdLibrary();

  /*!
  De-initialize gd library
  */
  ~AGdLibrary();
};

#endif //INCLUDED__AGdLibrary_HPP__

/*
Example:

#inclue "AGdCanvas.hpp"

AGdLibrary sg_GdLibraryInstance;

int main()
{
  AGdCanvas canvas;

  // do stuff

  return 0;
}
*/
