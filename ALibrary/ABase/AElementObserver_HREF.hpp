#ifndef INCLUDED__AElementObserver_HREF_HPP__
#define INCLUDED__AElementObserver_HREF_HPP__

#include "apiABase.hpp"
#include "AElementObserverInterface.hpp"

class AUrl;

class ABASE_API AElementObserver_HREF : public AElementObserverInterface
{
public:
  //a_ctor/dtor
  AElementObserver_HREF(AElementInterface *pElement, const AUrl *purlRequest);
  virtual ~AElementObserver_HREF();

  //a_Access to the generated URL
  virtual AUrl &getSubmissionUrl() { return _getSubmitUrl(); }

private:
  AElementObserver_HREF();         //a_Prevent construction

  //a_Generate URL from HREF and request URL
  void __generateSubmissionUrl();
};

#endif
