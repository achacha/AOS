/*
Written by Alex Chachanashvili

$Id: AOSOutput_AOSOutput_captcha_generate.hpp 95 2008-06-09 21:27:46Z achacha $
*/
#ifndef INCLUDED__AOSOutput_Captcha_generate_HPP__
#define INCLUDED__AOSOutput_Captcha_generate_HPP__

#include "apiAOS_User.hpp"
#include "AOSOutputGeneratorInterface.hpp"

class AOS_USER_API AOSOutput_Captcha_generate : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_Captcha_generate(AOSServices&);

  /*!
  Execute
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_Captcha_generate_HPP__
