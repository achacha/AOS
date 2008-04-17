#ifndef INCLUDED__AOSModule_FileList_HPP__
#define INCLUDED__AOSModule_FileList_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Get a list of files at a given path

Uses module/path + 'offset' from query string as base directory
Blocks relative path that goes below base
Uses module/path@base for all path values (does not allow query string to change base)

<module class="FileList">
  <path base="static|data|dynamic">
</module>

Default base is static.
*/
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
