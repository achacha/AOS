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
  //! Dataset element
  static const AString S_DATASET;

  //! Default to use
  static const AString   S_DEFAULT;

  //! Name of the query parameter override
  static const AString   S_QUERYOVERRIDE;

  //! Path to dataset data in the model
  static const AString DATASETPATH;

  //! Path to store globals for the given template
  static const AString GLOBALSPATH;
};

#endif
