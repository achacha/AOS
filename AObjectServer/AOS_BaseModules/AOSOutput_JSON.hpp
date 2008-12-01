/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_JSON_HPP__
#define INCLUDED__AOSOutput_JSON_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSOutput_JSON : public AOSOutputGeneratorInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSOutput_JSON(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_JSON_HPP__
