#include "pchABase.hpp"
#include "AElementObserver_HREF.hpp"
#include "AElement_HTML.hpp"
#include "AException.hpp"

AElementObserver_HREF::AElementObserver_HREF(AElementInterface *pElement, const AUrl *purlRequest) :
  AElementObserverInterface(pElement, purlRequest)
{
  //a_Check for types that use HREF as a target URL field
  if (!pElement->isType("a") &&
      !pElement->isType("area") )
    ATHROW(this, AException::InvalidParameter);

  //a_Parse the FORM
  __generateSubmissionUrl();
}

AElementObserver_HREF::~AElementObserver_HREF()
{
}

void AElementObserver_HREF::__generateSubmissionUrl()
{
  AElement_HTML* phe = dynamic_cast<AElement_HTML*>(_getHead());
  if (phe)
  {
    //a_Get the URL from the HREF
    AString strUrl;
    phe->getValue("href", strUrl);
    _getSubmitUrl().parse(strUrl);
  
    //a_Modify with respect to request URL
    const AUrl &request = _getRequestUrl();
    AUrl &submit = _getSubmitUrl();
    submit |= request;
  }
  else
    ATHROW(this, AException::TypeidMismatch);
}
