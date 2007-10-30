#ifndef INCLUDED__AOSModule_WikiViewFromFileSystem_HPP__
#define INCLUDED__AOSModule_WikiViewFromFileSystem_HPP__

#include "apiAOS_Wiki.hpp"

/*!
View Wiki data at wikipath parameter and base file system path
*/
class AOS_WIKI_API AOSModule_WikiViewFromFileSystem : public AOSModuleInterface
{
public:
  AOSModule_WikiViewFromFileSystem(AOSServices&);
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_WikiViewFromFileSystem_HPP__
