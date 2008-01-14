#ifndef INCLUDED__AElementObserver_FORM_HPP__
#define INCLUDED__AElementObserver_FORM_HPP__

#include "apiABase.hpp"
#include "AElementObserverInterface.hpp"
#include "AElement_HTML.hpp"

class AUrl;

class ABASE_API AElementObserver_FORM : public AElementObserverInterface
{
public:
  enum E_FormInputType
  {
    TYPE_IGNORE,             //a_RESET/FILE
    TYPE_TEXT,               //a_TEXT/PASSWORD
    TYPE_RADIO,
    TYPE_CHECKBOX,
    TYPE_HIDDEN,             //a_HIDDEN/SUBMIT
    TYPE_IMAGE,
    TYPE_SELECT,
    TYPE_TEXTAREA
  };

public:
  AElementObserver_FORM(AElementInterface *pElement, const AUrl *purlRequest);
  virtual ~AElementObserver_FORM();

  //a_Access methods
  void getMethod(AString&);
  void getAction(AString&);
  virtual AUrl &getSubmissionUrl() { return _getSubmitUrl(); }

private:
  //a_No default creation (ie. No blind observers needed!)
  AElementObserver_FORM();

  //a_Map type name to enum (see above)
  AElementObserver_FORM::E_FormInputType __mapInputType(AElement_HTML *peInput);
  
  //a_URL that will be used for submission
  void __generateSubmissionUrl();

  //a_URL preparation (accepts urlRequest and generates submission URL)
  void __prepareSubmissionURL();

  //a_Input generators
  void __addTextInput(AElement_HTML *pInputElement);
  void __addHiddenInput(AElement_HTML *pInputElement);
  void __addTextAreaInput(AElement_HTML *pInputElement);
  void __addImageInput(AElement_HTML *pInputElement);
  void __addRadioInput(AElement_HTML *pInputElement);
  void __addCheckboxInput(AElement_HTML *pInputElement);
  void __addSelectInput(AElement_HTML *pInputElement);
};

#endif


