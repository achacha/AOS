#ifndef INCLUDED__AElementObserver_SRC_HPP__
#define INCLUDED__AElementObserver_SRC_HPP__

#include "apiABase.hpp"
#include "AElementObserverInterface.hpp"
#include "AUrl.hpp"

class ABASE_API AElementObserver_SRC : public AElementObserverInterface
{
public:
  //a_ctor/dtor
  AElementObserver_SRC(AElementInterface *pElement, const AUrl *purlRequest);
  virtual ~AElementObserver_SRC();

  //a_Access to the generated URL
  virtual AUrl &getSubmissionUrl() { return _getSubmitUrl(); }

private:
  AElementObserver_SRC();         //a_Prevent default construction

  //a_Generate URL from HREF and request URL
  void __generateSubmissionUrl();
};

#endif
