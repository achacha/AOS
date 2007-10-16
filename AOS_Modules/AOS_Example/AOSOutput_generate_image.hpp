#ifndef INCLUDED__AOSOutput_generate_image_HPP__
#define INCLUDED__AOSOutput_generate_image_HPP__

#include "apiAOS_Example.hpp"
#include "AOSOutputGeneratorInterface.hpp"

class AOS_EXAMPLE_API AOSOutput_generate_image : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_generate_image(AOSServices&);
  
  virtual bool execute(AOSOutputContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSOutput_generate_image_HPP__
