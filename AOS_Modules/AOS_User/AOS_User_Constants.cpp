/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_User.hpp"
#include "AOS_User_Constants.hpp"

const AString AOS_User_Constants::USERNAME("un",2);
const AString AOS_User_Constants::PASSWORD("pwd",3);
const AString AOS_User_Constants::CAPTCHA("captcha",7);

const AString AOS_User_Constants::SESSION_USER("user",4);
const AString AOS_User_Constants::SESSION_REDIRECTURL("user/ru",7);
const AString AOS_User_Constants::SESSION_LOGINFAILCOUNT("user/failCount",14);
const AString AOS_User_Constants::SESSION_CAPTCHA("captcha",7);

const AString AOS_User_Constants::PARAM_REDIRECT_RETRYPAGE("redirect/retry",14);
const AString AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE("redirect/login",14);
const AString AOS_User_Constants::PARAM_REDIRECT_FAILURE("redirect/failure",16);
const AString AOS_User_Constants::PARAM_ATTEMPTS("attempts",8);
const AString AOS_User_Constants::PARAM_SECURE("secure",6);

const AString AOS_User_Constants::MODEL_CAPTCHA_VALID("captcha/Valid",13);
const AString AOS_User_Constants::MODEL_CAPTCHA_NOTVALID("captcha/NotValid",16);
