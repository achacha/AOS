/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_Redirect_HPP__
#define INCLUDED__AOSOutput_Redirect_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Redirect
*/
class AOS_BASEMODULES_API AOSOutput_Redirect : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_Redirect(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_Redirect_HPP__
