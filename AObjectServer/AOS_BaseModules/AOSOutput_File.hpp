/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_File_HPP__
#define INCLUDED__AOSOutput_File_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
  Output contents of a file

  <output type="File">
    <content-type desc="Optional MIME content type for the file, default is picked based on extension or text/html as a fallback"/>
    <path desc="Relative file path from base directory"/>
    <base desc="Base directory">{ data | dynamic | absolute | static }</base>
      static   - aos_static (default)
      data     - aos_data
      dynamic  - aos_dynamic
      absolute - the path is the fully qualified path to the file (a bit dangerous)
  </output>
*/

class AOS_BASEMODULES_API AOSOutput_File : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_File(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_File_HPP__
