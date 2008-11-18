/*
Written by Alex Chachanashvili

$Id: AOSResourceManager.cpp 252 2008-08-02 21:26:15Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSResourceManager.hpp"
#include "AOSServices.hpp"
#include "AOSContext.hpp"
#include "AFileSystem.hpp"
#include "AFile_Physical.hpp"

void AOSResourceManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_SessionHolderContainer={" << std::endl;
  for (CONTAINER::const_iterator cit = m_Resources.begin(); cit != m_Resources.end(); ++cit)
  {
    ADebugDumpable::indent(os, indent+2) << cit->first << "={" << std::endl;
    cit->second.debugDump(os, indent+3);
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSResourceManager::getClass() const
{
  static const AString CLASS("AOSResourceManager");
  return CLASS;
}

void AOSResourceManager::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);
  
  {
    for (CONTAINER::const_iterator cit = m_Resources.begin(); cit != m_Resources.end(); ++cit)
    {
      ARope rope;
      cit->second.getRoot().emit(rope, 0);
      adminAddProperty(
        eBase,
        cit->first,
        rope,
        AXmlElement::ENC_CDATASAFE
      );
    }
  }
}

AOSResourceManager::AOSResourceManager(AOSServices& services) :
  m_Services(services)
{
  _load();

  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSResourceManager::~AOSResourceManager()
{
}

void AOSResourceManager::_load()
{
  AFilename f(m_Services.useConfiguration().getAosBaseConfigDirectory());
  f.usePathNames().push_back(ASW("resources",9));
  
  AFileSystem::FileInfos files;
  if (AFileSystem::dir(f, files) > 0)
  {
    for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      AFile_Physical resource(it->filename);
      resource.open();
      
      AString locale;
      it->filename.emitFilenameNoExt(locale);
      AASSERT(this, locale.getSize() > 0);
      m_Resources[locale].fromAFile(resource);
      
      AString str("Loading resource file: ");
      str.append(locale);
      str.append('=');
      str.append(it->filename);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      m_Services.useLog().add(str, ALog::WARNING);
    }
  }
}

const AXmlDocument *AOSResourceManager::getResources(const AString& name) const
{
  CONTAINER::const_iterator cit = m_Resources.find(name);
  if (cit != m_Resources.end())
    return &(cit->second);
  else
    return NULL;
}

const AXmlDocument *AOSResourceManager::getResources(const LIST_AString& names) const
{
  for (LIST_AString::const_iterator citName = names.begin(); citName != names.end(); ++citName)
  {
    CONTAINER::const_iterator citDoc = m_Resources.find(*citName);
    if (citDoc != m_Resources.end())
      return &(citDoc->second);
  }
  return NULL;
}
