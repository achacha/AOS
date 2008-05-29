/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_Wiki_ViewFromFileSystem_HPP__
#define INCLUDED__AOSModule_Wiki_ViewFromFileSystem_HPP__

#include "apiAOS_Wiki.hpp"

/*!
View Wiki data at wikipath parameter and base file system path
*/
class AOS_WIKI_API AOSModule_Wiki_ViewFromFileSystem : public AOSModuleInterface
{
public:
  AOSModule_Wiki_ViewFromFileSystem(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_Wiki_ViewFromFileSystem_HPP__
