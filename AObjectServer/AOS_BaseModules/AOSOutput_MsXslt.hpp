/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_MsXslt_HPP__
#define INCLUDED__AOSOutput_MsXslt_HPP__

#ifdef __WINDOWS__
#include "apiAOS_BaseModules.hpp"
#include "ASync_CriticalSection.hpp"

/*!
XSL transform
*/
class AOS_BASEMODULES_API AOSOutput_MsXslt : public AOSOutputGeneratorInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSOutput_MsXslt(AOSServices&);
  virtual ~AOSOutput_MsXslt();

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

  //!Initialization
  virtual void init();
  
  //! Deinitialization
  virtual void deinit();

  /*!
  AOSAdminInterface
  */
  void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);

private:
  struct XslDocHolder
  {
    void *m_ComPtr;
    AString m_filename;
  };
  typedef std::map<AString, XslDocHolder> DocContainer;
  DocContainer m_Dox;
  bool m_IsCacheEnabled;
  ASync_CriticalSection m_CacheSync;

  //a_Wrapper to cache
  AOSOutput_MsXslt::XslDocHolder *_getXslDocHolder(const AFilename&);
  
  //a_Read Xslt file to the cache
  AOSOutput_MsXslt::XslDocHolder *_readXslFile(const AString&);
  void _dumpToFile(ARope&);
};

#endif //__WINDOWS__
#endif //INCLUDED__AOSOutput_MsXslt_HPP__
