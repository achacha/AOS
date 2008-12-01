/*
Written by Alex Chachanashvili

$Id: AOSResourceManager.hpp 252 2008-08-02 21:26:15Z achacha $
*/
#ifndef INCLUDED__AOSResourceManager_HPP__
#define INCLUDED__AOSResourceManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AXmlDocument.hpp"

class AOSServices;
class AOSContext;

class AOS_BASE_API AOSResourceManager : public AOSAdminInterface
{
public:
  static const AString CLASS;

public:
  //! ctor
  AOSResourceManager(AOSServices&);
  
  //! dtor
  virtual ~AOSResourceManager();

  /*!
  Access the resource XML data

  @param name of the resource document
  @return pointer to constant AXmlDocument, NULL if not found
  */
  const AXmlDocument *getResources(const AString& name) const;

  /*!
  Access the resource XML data
  Will iterate through teh list and return the first one found

  @param names to check, first one found will be returned
  @return pointer to constant AXmlDocument, NULL if not found
  */
  const AXmlDocument *getResources(const LIST_AString& names) const;

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Reference to services
  AOSServices& m_Services;

  //! Resource container
  typedef std::map<AString, AXmlDocument> CONTAINER;
  CONTAINER m_Resources;

  //! Initialize
  void _load();
};

#endif //INCLUDED__AOSResourceManager_HPP__
