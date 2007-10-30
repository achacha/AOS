#ifndef INCLUDED__AOSModule_WikiViewFromDatabase_HPP__
#define INCLUDED__AOSModule_WikiViewFromDatabase_HPP__

#include "apiAOS_Wiki.hpp"

/*!
View Wiki data at wikipath parameter from database
*/
class AOS_WIKI_API AOSModule_WikiViewFromDatabase : public AOSModuleInterface
{
public:
  AOSModule_WikiViewFromDatabase(AOSServices&);
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_WikiViewFromDatabase_HPP__
