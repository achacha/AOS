#ifndef INCLUDED__AOSOutput_XalanXslt_HPP__
#define INCLUDED__AOSOutput_XalanXslt_HPP__

#include "apiAOS_BaseModules.hpp"
#include "AXsl.hpp"

class AOS_BASEMODULES_API AOSOutput_XalanXslt : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_XalanXslt(AOSServices&);
  virtual ~AOSOutput_XalanXslt();

  /*!
  Generate output
  */
  virtual bool execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

  //! Initialization
  virtual void init();

private:
  AXsl m_xsl;

  //a_Force a dump to file
  void _dumpToFile(ARope&);
};

#endif //INCLUDED__AOSOutput_XalanXslt_HPP__
