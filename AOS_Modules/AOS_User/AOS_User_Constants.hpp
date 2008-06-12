/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOS_User_Constants_HPP__
#define INCLUDED__AOS_User_Constants_HPP__

#include "apiAOS_User.hpp"

/*!
Internal constants used by related modules in this library
*/
class AOS_USER_API AOS_User_Constants
{
public:
  /*!
  Session data parameters
  */
  static const AString SESSION_REDIRECTURL;
  static const AString SESSION_LOGINFAILCOUNT;
  static const AString SESSION_USER;
  static const AString SESSION_CAPTCHA;

  /*!
  Query parameters
  */
  static const AString USERNAME;
  static const AString PASSWORD;
  static const AString CAPTCHA;
  
  /*!
  Module parameters
  */
  static const AString PARAM_REDIRECT_RETRYPAGE;
  static const AString PARAM_REDIRECT_LOGINPAGE;
  static const AString PARAM_REDIRECT_LOGINPAGE_RELATIVEATTR;
  static const AString PARAM_REDIRECT_FAILURE;
  static const AString PARAM_ATTEMPTS;
  static const AString PARAM_SECURE;

  /*!
  Model path
  */
  static const AString MODEL_CAPTCHA_VALID;
  static const AString MODEL_CAPTCHA_NOTVALID;
};

#endif
