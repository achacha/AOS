/*
Written by Alex Chachanashvili

$Id: AOS_User_Constants.hpp 97 2008-06-12 17:59:55Z achacha $
*/
#ifndef INCLUDED__AOS_DadaData_Constants_HPP__
#define INCLUDED__AOS_DadaData_Constants_HPP__

#include "apiAOS_DadaData.hpp"

/*!
Internal constants used by related modules in this library
*/
class AOS_DADADATA_API AOS_DadaData_Constants
{
public:
  //! Extension to simulate when setting content-type (looks up content-type mapping for a given extension)
  static const AString MIME_EXTENSION;

  //! Template
  static const AString TEMPLATE;

  //! Filename
  static const AString FILENAME;
};

#endif
