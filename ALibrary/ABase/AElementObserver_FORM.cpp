#include "pchABase.hpp"
#include "AElementObserver_FORM.hpp"
#include "AUrl.hpp"
#include "ATextGenerator.hpp"
#include "AException.hpp"

AElementObserver_FORM::AElementObserver_FORM(AElementInterface *pElement, const AUrl *purlRequest) :
  AElementObserverInterface(pElement, purlRequest)
{
  //a_Element must be a FORM type
  if (!pElement->isType("FORM"))
    ATHROW(this, AException::InvalidParameter);

  //a_Parse the FORM
  __generateSubmissionUrl();
}

AElementObserver_FORM::E_FormInputType AElementObserver_FORM::__mapInputType(AElement_HTML *peInput)
{
  //a_First input types in NAME=type  (HTML default is TEXT)
  //a_HTML types in <type ...>
  AString strType = peInput->getType();
  if (strType.isEmpty() == FALSE)
  {
    if (!strType.compareNoCase("INPUT"))
    {
      strType = "TEXT";
      peInput->getValue("TYPE", strType);
      if (!strType.isEmpty())
      {
        if      (!strType.compareNoCase("TEXT"))     return AElementObserver_FORM::TYPE_TEXT;
        else if (!strType.compareNoCase("HIDDEN"))   return AElementObserver_FORM::TYPE_HIDDEN;
        else if (!strType.compareNoCase("CHECKBOX")) return AElementObserver_FORM::TYPE_CHECKBOX;
        else if (!strType.compareNoCase("SUBMIT"))   return AElementObserver_FORM::TYPE_HIDDEN;
        else if (!strType.compareNoCase("RADIO"))    return AElementObserver_FORM::TYPE_RADIO;
        else if (!strType.compareNoCase("PASSWORD")) return AElementObserver_FORM::TYPE_TEXT;
        else if (!strType.compareNoCase("IMAGE"))    return AElementObserver_FORM::TYPE_IMAGE;
      }
    }
    else if (!strType.compareNoCase("TEXTAREA")) return AElementObserver_FORM::TYPE_TEXTAREA;
    else if (!strType.compareNoCase("SELECT"))   return AElementObserver_FORM::TYPE_SELECT;
  }

  //a_Default is to ignore
  return AElementObserver_FORM::TYPE_IGNORE;
}


AElementObserver_FORM::~AElementObserver_FORM()
{
}

void AElementObserver_FORM::__generateSubmissionUrl()
{
  //a_Reset the URL object
  _getSubmitUrl().clear();
  
  //a_Given the last URL, the new one will be prepared with correct method, action and such
  __prepareSubmissionURL();

  //a_Find FORMs and INPUTs
  AElement_HTML *pInputElement = NULL;

  //a_Find all the FORM  items
  while (pInputElement = dynamic_cast<AElement_HTML*>(_getHead())->findFormItem(pInputElement))
  {
    //a_Generate submission variables
    switch(__mapInputType(pInputElement))
    {
      case AElementObserver_FORM::TYPE_IGNORE :
      break;

      case AElementObserver_FORM::TYPE_TEXT :
        __addTextInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_HIDDEN :
        __addHiddenInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_TEXTAREA :
        __addTextAreaInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_IMAGE :
        __addImageInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_RADIO :
        __addRadioInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_CHECKBOX :
        __addCheckboxInput(pInputElement);
      break;

      case AElementObserver_FORM::TYPE_SELECT :
        __addSelectInput(pInputElement);
      break;

      default :
        ATHROW_EX(this, AException::InvalidParameter, ASW("Invalid form type: ",19)+pInputElement->beginElement());
    }
  }
}

void AElementObserver_FORM::__prepareSubmissionURL()
{
  //a_Action exsts
  AString strAction;
  getAction(strAction);
  _getSubmitUrl().parse(strAction);
  if (_getSubmitUrl().getServer() == strAction)
  {
    //a_Single name was used and is probably the CGI excutable
    //a_An anomaly of URLs
    //a_e.g.  ACTION="myfile.exe"  is that http://myfile.exe/  or myfile.exe the CGI? and EXE can be a domain in the future
    //a_                           or form.pl, is .pl Poland?? since http://form.pl/index.cgi can be defaulted to by the server
    _getSubmitUrl() = _getRequestUrl();
    _getSubmitUrl().setFilename(strAction);
  }
  else
  {
    //a_Fill in missing information from request URL
    const AUrl &request = _getRequestUrl();
    AUrl &submit = _getSubmitUrl();
    submit |= request;
  }
}

void AElementObserver_FORM::__addTextInput(AElement_HTML *pInputElement)
{
  //a_No NAME, no input
  if (pInputElement->isName("NAME"))
  {
    //a_See if maximum length was specified
    AString str("128");
    pInputElement->getValue("MAXLENGTH", str);
    int iMaxLength = str.toInt();
    int iLength = ARandomNumberGenerator::get().nextRange(13, 4);
    if (iMaxLength < iLength) iLength = iMaxLength;

    //a_Create a random word
    AString strWord;
    ATextGenerator::generateRandomString(strWord, iLength);
    AString strName("Name");
    pInputElement->getValue("NAME", strName);
    _getSubmitUrl().useParameterPairs().insert(strName, strWord);
  }
}

void AElementObserver_FORM::__addHiddenInput(AElement_HTML *pInputElement)
{
  //a_Resubmit HIDDEN fields... No NAME, no input
  if (pInputElement->isName("NAME"))
  {
    AString strName("Name"), strValue;
    pInputElement->getValue("NAME", strName);
    pInputElement->getValue("VALUE", strValue);
    _getSubmitUrl().useParameterPairs().insert(strName, strValue);
  }
}

void AElementObserver_FORM::__addTextAreaInput(AElement_HTML *pInputElement)
{
  //a_No NAME, no input
  if (pInputElement->isName("NAME"))
  {
    //a_Create a random word (10 characters)
    AString strSentence;
    for (int iWords = 0; iWords < 3; iWords++)
    {
       ATextGenerator::generateRandomString(strSentence, 10);
       strSentence.append(' ');
    }

    strSentence += AString::sstr_CRLF;  //HTML 3.2 spec states that \r\n is needed when submitting TEXTAREA text
  
    AString strName;
    pInputElement->getValue("NAME", strName);
    _getSubmitUrl().useParameterPairs().insert(strName, strSentence);
  }
}

void AElementObserver_FORM::__addImageInput(AElement_HTML *pInputElement)
{
  //a_Submit pair if NAME exists
  if (pInputElement->isName("NAME"))
  {
    AString strName;
    pInputElement->getValue("NAME", strName);
    _getSubmitUrl().useParameterPairs().insert(strName + ".X", AString::sstr_Zero);
    _getSubmitUrl().useParameterPairs().insert(strName + ".Y", AString::sstr_Zero);
  }
}

void AElementObserver_FORM::__addRadioInput(AElement_HTML *pInputElement)
{
  //a_No NAME, no input
  if (pInputElement->isName("NAME"))
  {
    AString strName;
    pInputElement->getValue("NAME", strName);
    if (!_getSubmitUrl().useParameterPairs().count(strName))
    {
      //a_Add it, one doesn't exist (assumes at least one radio button must be selected)
      AString strValue(AString::sstr_One);
      pInputElement->getValue("VALUE", strValue);
      _getSubmitUrl().useParameterPairs().insert(strName, strValue);
    }
    else
    {
      //a_Some percentage base (really arbitrary)
      if (ARandomNumberGenerator::get().nextRange(1000) > 666)
      {
        //a_33% chance of being added
        AString strValue(AString::sstr_One);
        pInputElement->getValue("VALUE", strValue);
        _getSubmitUrl().useParameterPairs().insert(strName, strValue);
      }
    }
  }
}

void AElementObserver_FORM::__addCheckboxInput(AElement_HTML *pInputElement)
{
  //a_50% chance of being selected
  if (pInputElement->isName("NAME"))
    if (ARandomNumberGenerator::get().nextRange(1000) > 500)
    {
      AString strName, strValue(AString::sstr_One);
      pInputElement->getValue("NAME", strName);
      pInputElement->getValue("VALUE", strValue);
      _getSubmitUrl().useParameterPairs().insert(strName, strValue);
    }
}

void AElementObserver_FORM::__addSelectInput(AElement_HTML *pInputElement)
{
  //a_No NAME, no input
  if (pInputElement->isName("NAME"))
  {
    AElement_HTML *peOption = pInputElement, *peUse = NULL;
    while ( peOption = pInputElement->findType("OPTION", peOption))
    {
      if (peUse == NULL)
      {
        //a_No option yet
        peUse = peOption;
      }
      else
      {
        if (ARandomNumberGenerator::get().nextRange(1000) > 666)
          peUse = peOption;
      }
    }

    if (peUse)
    {
      AString strName, strValue(AString::sstr_Null);
      pInputElement->getValue("NAME", strName);
      peUse->getValue("VALUE", strValue);
      _getSubmitUrl().useParameterPairs().insert(strName, strValue);
    }
  }
}

void AElementObserver_FORM::getMethod(AString& str)
{ 
  AElement_HTML* phe = dynamic_cast<AElement_HTML*>(_getHead());
  if (phe)
    phe->getValue("METHOD", str);
  else
    ATHROW(this, AException::TypeidMismatch);
}

void AElementObserver_FORM::getAction(AString& str)
{
  AElement_HTML* phe = dynamic_cast<AElement_HTML*>(_getHead());
  if (phe)
    phe->getValue("ACTION", str);
  else
    ATHROW(this, AException::TypeidMismatch);
}
