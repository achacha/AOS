#include "pchAOS_Example.hpp"
#include "AOSModule_classified_input.hpp"

const AString& AOSModule_classified_input::getClass() const
{
  static const AString CLASS("classified_input");
  return CLASS;
}

AOSModule_classified_input::AOSModule_classified_input(AOSServices& services) :
  AOSModuleInterface(services)
{
}

/*
<table><tr><th>
	<img 
		align="top" 
		src="images/minus_16x16.png" 
		onclick="var x=getHttpXml('classified_contact_email', 0); this.style.visibility='hidden'; return x;"
	/><span class="feature_on">Contact Email</span>
</th><td>
	<input type="text" name="contact_email" length="20" size="20"/>
</td></tr></table>

<table><tr><td colspan="2">
	<img 
		align="top" 
		src="images/plus_16x16.png" 
		onclick="var x=getHttpXml('clasified_contact_email', 1); this.style.visibility='hidden'; return x;"
	/><span class="feature_off">Contact Email</span>
</td></tr></table>
*/
bool AOSModule_classified_input::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AString str;
  bool state = false;
  const AXmlNode *pNode = NULL;
  
  if (pNode = moduleParams.findNode(ASW("/params/module/state",20)))
  {
    pNode->emitContent(str);
    if (str.at(0) == '1')
      state = true;
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_input::execute"), ASWNL("classified_input module missing '/params/module/state' parameter"));
    return false;
  }

  AString strDisplay;
  if (pNode = moduleParams.findNode(ASW("/params/module/display",22)))
  {
    pNode->emitContent(strDisplay);
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_input::execute"), ASWNL("classified_input module missing '/params/module/display' parameter"));
    return false;
  }

  AString strVariable;
  if (pNode = moduleParams.findNode(ASW("/params/module/variable",23)))
  {
    pNode->emitContent(strVariable);
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_input::execute"), ASWNL("classified_input module missing '/params/module/variable' parameter"));
    return false;
  }

  AString strX("var x=getHttpXml('");
  strX.append(strVariable);
  strX.append("',",2);
  strX.append(state ? '0' : '1');
  strX.append(",'",2);
  strX.append(strDisplay);
  strX.append("'); this.style.visibility='hidden'; return x;");

  context.useOutputRootXmlElement().useName().assign("table",5);
  AXmlElement& eTR = context.useOutputRootXmlElement().addElement(ASW("tr",2));
  
  AXmlElement& th0 = eTR.addElement(ASW("th",2));
  th0.addElement("img")
    .addAttribute("align", "top")
    .addAttribute("src", (state ? "images/minus_16x16.png" : "images/plus_16x16.png"))
    .addAttribute("onclick", strX);

  strX.assign("feature_");
  strX.append(state ? "on" : "off");
  th0.addElement("span", strDisplay)
    .addAttribute("class", strX);

  if (state)
  {
    AString strLength;
    if (!context.useRequestParameterPairs().get(ASW("length",6), strLength))
      strLength.assign("20", 2);
    
    eTR.addElement("td")
      .addElement("input")
      .addAttribute("type", "text")
      .addAttribute("name", strVariable)
      .addAttribute("maxlength", strLength)
      .addAttribute("size", strLength);
  }
  
  return true;
}

