/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSInput_HtmlForm_HPP__
#define INCLUDED__AOSInput_HtmlForm_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSInput_HtmlForm : public AOSInputProcessorInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSInput_HtmlForm(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSInput_HtmlForm_HPP__
