/*
Written by Alex Chachanashvili

$Id: AOSOutput_XML.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSOutput_XML_HPP__
#define INCLUDED__AOSOutput_XML_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSOutput_XML : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_XML(AOSServices&);

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_XML_HPP__
