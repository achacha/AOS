#include "pchABase.hpp"
#include "AElementObserver_SRC.hpp"
#include "AElement_HTML.hpp"
#include "AException.hpp"

AElementObserver_SRC::AElementObserver_SRC(AElementInterface *pElement, const AUrl *purlRequest) :
  AElementObserverInterface(pElement, purlRequest)
{
  //a_Check for a valid types that use SRC to contain a target URL
  if (!pElement->isType("frame") &&
      !pElement->isType("img") &&
      !pElement->isType("embed")
     )
    ATHROW(this, AException::InvalidParameter);

  //a_Parse the FORM
  __generateSubmissionUrl();
}

AElementObserver_SRC::~AElementObserver_SRC()
{
}

void AElementObserver_SRC::__generateSubmissionUrl()
{
  //a_Get the URL from the HREF
  AElement_HTML* phe = dynamic_cast<AElement_HTML*>(_getHead());
  if (phe)
  {
    AString strSrc;
    phe->getValue("SRC", strSrc);
    _getSubmitUrl().parse(strSrc);
  
    //a_Modify with respect to request URL
    const AUrl &request = _getRequestUrl();
    AUrl &submit = _getSubmitUrl();
    submit |= request;
  }
  else
    ATHROW(this, AException::TypeidMismatch);
}
