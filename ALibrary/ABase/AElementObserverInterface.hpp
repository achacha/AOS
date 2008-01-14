#ifndef INCLUDED__AElementObserverInterface_HPP__
#define INCLUDED__AElementObserverInterface_HPP__

#include "apiABase.hpp"
#include "AElementInterface.hpp"
#include "AUrl.hpp"
#include "ADebugDumpable.hpp"

class ABASE_API AElementObserverInterface : public ADebugDumpable
{
public:
  //a_ctor/dtor
  AElementObserverInterface(AElementInterface *pElement, const AUrl *purlRequest);
  virtual ~AElementObserverInterface();

  //a_Access to the generated URL
  virtual AUrl &getSubmissionUrl() = 0;
  
  //a_Access to type
  AElementInterface *getElement() const { return m__peHead; }
  const AString &getType() const { return m__peHead->getType(); }

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //a_Element MUST exist and be valid, or else bad things will happend
  AElementInterface *_getHead() { return m__peHead; }

  //a_Access to the URL
  const AUrl &_getRequestUrl() { return *m__purlRequest; }
  AUrl &_getSubmitUrl() { return m__urlSubmit;  }

private:
  AElementObserverInterface();        //a_Prevent default construction
  
  //a_The subject element
  AElementInterface *m__peHead;       //a_Pointer to the observed element

  //a_URL used to aquire this element (the AHtmlPage object that is)
  const AUrl *m__purlRequest;         //a_Pointer to the AUrl that generated this element
  AUrl        m__urlSubmit;           //a_Submission AUrl that is based on the request URL
};

#endif


