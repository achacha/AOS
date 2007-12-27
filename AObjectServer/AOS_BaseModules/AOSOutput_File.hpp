#ifndef INCLUDED__AOSOutput_File_HPP__
#define INCLUDED__AOSOutput_File_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
  Output contents of a file

  <file>
    <content-type desc="MIME content type for the file"/>
    <path desc="Relative file path from aos_static directory"/>
    <base desc="Base directory">{ data | dynamic | absolute | static }</base>
      data     - aos_data  (default)
      dynamic  - aos_dynamic
      static   - aos_static
      absolute - the path is the fully qualified path to the file (a bit dangerous)
  </file>
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
