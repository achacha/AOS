/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_Redirect_HPP__
#define INCLUDED__AOSOutput_Redirect_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
  Output redirected
  'secure' true or false will force https: and http: respectively
  Not including secure will do nothing to the protocol

  Redirect to relative or absolute URL
  <output>
    <redirect>http://somwhere/path/file?query</redirect>
    <secure>true</secure>
  </output>

  Redirect to a URL in the model
  <output>
    <redirect reference="MODEL">/root/redirectUrl</redirect>
    <secure>false</secure>
  </output>

  Redirect to a URL in the session data 
    or URL in the model if session data not found
    or referer from HTTP request if model data not found
  <output>
    <redirect reference="SESSION">/root/redirectUrl</redirect>
    <redirect reference="MODEL">/root/redirectUrl</redirect>
    <redirect reference="REFERER"/>
    <secure>false</secure>
  </output>
*/

class AOS_BASEMODULES_API AOSOutput_Redirect : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_Redirect(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_Redirect_HPP__
