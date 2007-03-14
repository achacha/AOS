#ifndef INCLUDED_AOSModule_BlogView_HPP_
#define INCLUDED_AOSModule_BlogView_HPP_

#include "apiAOS_Blog.hpp"

class AOS_BLOG_API AOSModule_BlogView : public AOSModuleInterface
{
public:
  AOSModule_BlogView(ALog&);
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED_AOSModule_classified_input_HPP_
