#ifndef INCLUDED__AOS_User_Constants_HPP__
#define INCLUDED__AOS_User_Constants_HPP__

#include "apiAOS_User.hpp"

/*!
Internal constants used by related modules in this library
*/
class AOS_USER_API AOS_User_Constants
{
public:
  static const AString USERNAME;
  static const AString PASSWORD;

  static const AString SESSION_REDIRECTURL;
  static const AString SESSION_ISLOGGEDIN;
  static const AString SESSION_LOGINFAILCOUNT;

  static const AString PARAM_REDIRECT_LOGINPAGE;
  static const AString PARAM_REDIRECT_FAILURE;
  static const AString PARAM_ATTEMPTS;
  static const AString PARAM_SECURE;
};

#endif
