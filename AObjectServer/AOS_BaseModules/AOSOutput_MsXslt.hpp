#ifndef INCLUDED__AOSOutput_MsXslt_HPP__
#define INCLUDED__AOSOutput_MsXslt_HPP__

#ifdef AOS__USE_MSXML4__
#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSOutput_MsXslt : public AOSOutputGeneratorInterface
{
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

private:
  struct XslDocHolder
  {
    void *m_ComPtr;
    AString m_filename;
  };
  typedef std::map<AString, XslDocHolder> DocContainer;
  DocContainer m_Dox;

  //a_Read Xslt file to the cache
  AOSOutput_MsXslt::XslDocHolder *_readXslFile(const AString& filename);
  void _dumpToFile(ARope&);
};

#endif //AOS__USE_MSXML4__
#endif //INCLUDED__AOSOutput_MsXslt_HPP__
