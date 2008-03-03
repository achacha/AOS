#ifndef INCLUDED__AOSInput_HtmlFormMultiPart_HPP__
#define INCLUDED__AOSInput_HtmlFormMultiPart_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Parse the multi-part post and place result into specified destination as a model element

<input class="multipart/form-data"/>

Objects read as file type are stored in the request objects container and their name is stored as value in the query parameter pair
Existing objects are overwritten.

To access data stored (if your file input name="fileinput"):
  AString objectname;
  if (context.useRequestParameterPairs().get(ASW("fileinput",9), objectname)
  {
    AString *pData = context.useContextObjects().getAsPtr<AString>(objectname);
    // Now pData is (AString *) that contains the file data submitted
  }
*/
class AOS_BASEMODULES_API AOSInput_HtmlFormMultiPart : public AOSInputProcessorInterface
{
public:
  AOSInput_HtmlFormMultiPart(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSInput_HtmlForm_HPP__
