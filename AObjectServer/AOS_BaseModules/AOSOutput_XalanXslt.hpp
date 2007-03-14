#ifndef INCLUDED__AOSOutput_XalanXslt_HPP__
#define INCLUDED__AOSOutput_XalanXslt_HPP__

#include "apiAOS_BaseModules.hpp"
#include "AXsl.hpp"

class AOS_BASEMODULES_API AOSOutput_XalanXslt : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_XalanXslt(ALog&);
  virtual ~AOSOutput_XalanXslt();
  virtual bool execute(AOSOutputContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

  //a_Initialization
  virtual void init(AOSServices&);

private:
  AXsl m_xsl;

  //a_Force a dump to file
  void _dumpToFile(ARope&);
};

#endif //INCLUDED__AOSOutput_XalanXslt_HPP__
