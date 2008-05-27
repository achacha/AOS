/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSInput_NOP_HPP__
#define INCLUDED__AOSInput_NOP_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSInput_NOP : public AOSInputProcessorInterface
{
public:
  AOSInput_NOP(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSInput_NOP_HPP__
