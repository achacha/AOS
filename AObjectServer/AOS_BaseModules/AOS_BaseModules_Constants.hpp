/*
Written by Alex Chachanashvili

$Id: AOS_User_Constants.hpp 97 2008-06-12 17:59:55Z achacha $
*/
#ifndef INCLUDED__AOS_BaseModules_Constants_HPP__
#define INCLUDED__AOS_BaseModules_Constants_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Internal constants used by related modules in this library
*/
class AOS_BASEMODULES_API AOS_BaseModules_Constants
{
public:
  static const AString SECURE;

  //! Element that refers to a file and file base directory
  static const AString FILENAME;
  static const AString BASE;
  
  //! Usually path into the model
  static const AString PATH;

  //! SQL
  static const AString SQL;

  //! Extension to simulate when setting content-type (looks up content-type mapping for a given extension)
  static const AString MIME_EXTENSION;

  //! String for file
  static const AString S_FILE;

  //! Template
  static const AString TEMPLATE;
};

#endif
