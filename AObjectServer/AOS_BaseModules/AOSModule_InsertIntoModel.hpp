/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSModule_InsertIntoModel_HPP__
#define INCLUDED__AOSModule_InsertIntoModel_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Insert XML directly into the model

<module class="InsertIntoModel">
  <data>
    <foo>Some data</foo>
  </data>
  <path>/import-node</path>
</module>

Result in model will be:

<root>
  <import-node>
    <foo>Some data</foo>
  </import-node>
  ...
</root>

*/
class AOS_BASEMODULES_API AOSModule_InsertIntoModel : public AOSModuleInterface
{
public:
  AOSModule_InsertIntoModel(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_InsertIntoModel_HPP__
