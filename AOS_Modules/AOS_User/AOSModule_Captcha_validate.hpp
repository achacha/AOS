/*
Written by Alex Chachanashvili

$Id: AOSModule_rss20.hpp 89 2008-05-29 23:25:42Z achacha $
*/
#ifndef INCLUDED__AOSModule_Captcha_validate_HPP__
#define INCLUDED__AOSModule_Captcha_validate_HPP__

#include "apiAOS_User.hpp"
#include "AOSModuleInterface.hpp"

class AOS_USER_API AOSModule_Captcha_validate : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_Captcha_validate(AOSServices&);
  
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_Captcha_validate_HPP__
