#ifndef INCLUDED__AOSOutput_Redirect_HPP__
#define INCLUDED__AOSOutput_Redirect_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
  Output redirected
  'secure' true or false will force https: and http: respectively

  Redirect to relative or absolute URL
  <output>
    <redirect-url>http://somwhere/path/file?query</redirect-url>
    <secure>true</secure>
  </output>

  Redirect to a URL in the model
  <output>
    <redirect-path>/root/redirectUrl</redirect-path>
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
