#ifndef INCLUDED__AOSModule_WikiView_HPP__
#define INCLUDED__AOSModule_WikiView_HPP__

#include "apiAOS_Wiki.hpp"

/*!
View WIki data at wikipath parameter
*/
class AOS_WIKI_API AOSModule_WikiView : public AOSModuleInterface
{
public:
  AOSModule_WikiView(AOSServices&);
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_WikiView_HPP__
