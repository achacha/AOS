/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED_AOSModule_forum_list_HPP_
#define INCLUDED_AOSModule_forum_list_HPP_

#include "apiAOS_Forum.hpp"

class AOS_FORUM_API AOSModule_forum_list : public AOSModuleInterface
{
public:
  AOSModule_forum_list(AOSServices&);
  virtual ~AOSModule_forum_list();

  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED_AOSModule_classified_input_HPP_
