/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_File_HPP__
#define INCLUDED__AOSOutput_File_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
File output
*/
class AOS_BASEMODULES_API AOSOutput_File : public AOSOutputGeneratorInterface
{
public:
  //! Class name
  static const AString CLASS;

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
