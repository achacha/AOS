#ifndef INCLUDED__AOSInput_HtmlForm_HPP__
#define INCLUDED__AOSInput_HtmlForm_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSInput_HtmlForm : public AOSInputProcessorInterface
{
public:
  AOSInput_HtmlForm(ALog&);
  virtual bool execute(AOSContext&);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSInput_HtmlForm_HPP__
