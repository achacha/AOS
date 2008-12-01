/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_Wiki_ViewFromDatabase_HPP__
#define INCLUDED__AOSModule_Wiki_ViewFromDatabase_HPP__

#include "apiAOS_Wiki.hpp"

/*!
View Wiki data at wikipath parameter from database
*/
class AOS_WIKI_API AOSModule_Wiki_ViewFromDatabase : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_Wiki_ViewFromDatabase(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_Wiki_ViewFromDatabase_HPP__
