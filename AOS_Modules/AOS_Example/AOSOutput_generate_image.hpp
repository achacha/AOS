/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_generate_image_HPP__
#define INCLUDED__AOSOutput_generate_image_HPP__

#include "apiAOS_Example.hpp"
#include "AOSOutputGeneratorInterface.hpp"

class AOS_EXAMPLE_API AOSOutput_generate_image : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_generate_image(AOSServices&);
  
  /*!
  Initialize
  */
  virtual void init();

  /*!
  Execute
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

private:
  AString m_FontPath;
};

#endif //INCLUDED__AOSOutput_generate_image_HPP__
