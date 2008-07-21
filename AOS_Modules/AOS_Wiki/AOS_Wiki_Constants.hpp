/*
Written by Alex Chachanashvili

$Id: AOS_Wiki_Constants.hpp 97 2008-06-12 17:59:55Z achacha $
*/
#ifndef INCLUDED__AOS_Wiki_Constants_HPP__
#define INCLUDED__AOS_Wiki_Constants_HPP__

#include "apiAOS_Wiki.hpp"

/*!
Internal constants used by related modules in this library
*/
class AOS_WIKI_API AOS_Wiki_Constants
{
public:
  /*!
  Model path
  */
  static const AString ELEMENT_SECURE_EDIT;
  static const AString ELEMENT_DOES_NOT_EXIST;
  static const AString ELEMENT_DATA;
  
  /*!
  Query parameters
  */
  static const AString PARAM_SECURE;
  static const AString PARAM_BASE_PATH;
};

#endif
