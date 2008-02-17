#ifndef INCLUDED__AOSModule_FileList_HPP__
#define INCLUDED__AOSModule_FileList_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_FileList : public AOSModuleInterface
{
public:
  AOSModule_FileList(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_FileList_HPP__