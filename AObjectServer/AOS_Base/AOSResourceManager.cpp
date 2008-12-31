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

const AString AOSResourceManager::CLASS("AOSResourceManager");

const AString& AOSResourceManager::getClass() const
{
  return CLASS;
}

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
  // data.* query
  AFilename fData(m_Services.useConfiguration().getAosBaseDataDirectory());
  fData.usePathNames().pop_back();
  fData.useFilename().assign("data*.*", 7);

  AFileSystem::FileInfos files;
  if (AFileSystem::dir(fData, files, false, false) > 0)
  {
    AString locale;
    for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->typemask & AFileSystem::Directory)
      {
        const AString& dirName = it->filename.usePathNames().back();
        size_t pos = dirName.rfind('.');
        if (AConstant::npos == pos)
        {
          // Default locale
          locale.assign(m_Services.useConfiguration().getBaseLocale());
        }
        else
        {
          locale.clear();
          dirName.peek(locale, pos+1);
        }

        // Check if resource.xml exists for this data directory
        it->filename.useFilename().assign("resource.xml", 12);
        if (AFileSystem::exists(it->filename))
        {
          // Exists, load it
          AFile_Physical resource(it->filename);
          resource.open();
          
          AASSERT(this, locale.getSize() > 0);
          AASSERT(this, m_Resources.end() == m_Resources.find(locale));
          m_Resources[locale].fromAFile(resource);
          
          AString str("Loading resource file: ");
          str.append(locale);
          str.append('=');
          str.append(it->filename);
          AOS_DEBUGTRACE(str.c_str(), NULL);
          m_Services.useLog().add(str, ALog::EVENT_WARNING);
        }
        else
        {
          // Does not have a resource file
          AString str("No resource.xml found: ");
          str.append(it->filename);
          AOS_DEBUGTRACE(str.c_str(), NULL);
          m_Services.useLog().add(str, ALog::EVENT_DEBUG);
        }
      }
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
