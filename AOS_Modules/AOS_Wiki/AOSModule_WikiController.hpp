/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSModule_WikiController_HPP__
#define INCLUDED__AOSModule_WikiController_HPP__

#include "apiAOS_Wiki.hpp"

/*!
  Take an input URL: http://someserver.com/wiki/topic/subtopic/item
  Remap it into: http://someserver.com/wiki/wiki.html?wikipath=topic/subtopic/item

*/
class AOS_WIKI_API AOSModule_WikiController : public AOSModuleInterface
{
public:
  AOSModule_WikiController(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_WikiController_HPP__
