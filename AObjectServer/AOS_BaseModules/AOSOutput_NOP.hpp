/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSOutput_NOP_HPP__
#define INCLUDED__AOSOutput_NOP_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSOutput_NOP : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_NOP(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_NOP_HPP__
