/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__APythonEmbed_HPP__
#define INCLUDED__APythonEmbed_HPP__

#include "apiAPythonEmbed.hpp"
#include "AString.hpp"
#include <map>

class AObjectContainer; 

/*!
This object is meant to be global and called as needed
*/
class APYTHONEMBED_API APythonEmbed
{
public:
  /*!
  Initialize python interpreter and bind ALibrary methods
  */
  APythonEmbed();
  
  /*!
  Initialize python interpreter
  Requires the name of the program (usually argv[0])
  */
  bool initialize(char *);

  /*!
  De-initialize and unload the python interpreter
  */
  ~APythonEmbed();

  /*!
  Execute Python code
  */
  void execute(const AEmittable&);

  /*!
  AObjectContainer helper functions that can be called from the python exported ones
  */
  static void registerObjectContainer(const AString& name, AObjectContainer&);            //a_Register a reference to AObjectContainer
  static bool lookup(const AString& name, const AString& path, AOutputBuffer& target);

private:
  typedef std::map<AString, AObjectContainer *> OBJECTS;
  static OBJECTS sm_Objects;
};

#endif //INCLUDED__APythonEmbed_HPP__
