/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_SaveToFile_HPP__
#define INCLUDED__AOSModule_SaveToFile_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Save model element to file relative to the aos_data directory

Contents of the element saved to file
<module>
  <model-path>/root/something</model-path>
  <output-filename>path/somefile.txt</output-filename>
</module>

Contexts of the elements are used to lookup an object in ContextObjects
  then cast to AEmittable and output written to file
This is a way to get data parsed by multi-part-form, since the name of the
object is stored in the query string and data in put into the AOSContext.useContextObjects()
<module>
  <model-path reference="context-object">/root/REQUEST/url/query-string/file</model-path>
  <output-filename>path/somefile.txt</output-filename>
</module>
*/
class AOS_BASEMODULES_API AOSModule_SaveToFile : public AOSModuleInterface
{
public:
  AOSModule_SaveToFile(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_SaveToFile_HPP__
