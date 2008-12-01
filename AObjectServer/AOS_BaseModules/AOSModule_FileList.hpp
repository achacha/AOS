/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_FileList_HPP__
#define INCLUDED__AOSModule_FileList_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Get a list of files at a given path
*/
class AOS_BASEMODULES_API AOSModule_FileList : public AOSModuleInterface
{
public:
  //! Class name
  static const AString MODULE_CLASS;

public:
  AOSModule_FileList(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_FileList_HPP__
