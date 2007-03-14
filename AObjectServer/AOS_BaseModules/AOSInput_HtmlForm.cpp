#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlForm.hpp"

const AString& AOSInput_HtmlForm::getClass() const
{
  static const AString CLASS("application/x-www-form-urlencoded");
  return CLASS;
}

AOSInput_HtmlForm::AOSInput_HtmlForm(ALog& alog) :
  AOSInputProcessorInterface(alog)
{
}

bool AOSInput_HtmlForm::execute(AOSContext& context)
{
  AString str;
  
  //a_Read FORM data if any
  if (context.useRequestHeader().find(AHTTPHeader::HT_ENT_Content_Length, str))
  {
    //a_Content specified
    u4 toRead = str.toU4();
    str.clear();
    context.useSocket().read(str, toRead);
    
    //a_Add POST variables to request query
    AString strR(str.getSize(), 128);
    ATextConverter::decodeURL(str, strR);
    context.useRequestParameterPairs().parse(strR);
    
    //a_Publish raw_data if dumping context in debug mode and when 'dumpContext' exists
#ifdef __DEBUG_DUMP__
    if (context.useRequestParameterPairs().exists(ASW("dumpContext",11)))
      context.useOutputRootXmlElement().addElement(ASW("/input/data", 11), str, AXmlData::CDataSafe);
#endif
  }

  return true;
}
