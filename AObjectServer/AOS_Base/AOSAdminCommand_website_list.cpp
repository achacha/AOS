/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_website_list.hpp"
#include "AXmlElement.hpp"

const AString AOSAdminCommand_website_list::COMMAND("website_list",12);

const AString& AOSAdminCommand_website_list::getName() const
{
  return COMMAND;
}

AOSAdminCommand_website_list::AOSAdminCommand_website_list(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
}

void AOSAdminCommand_website_list::_process(AOSAdminCommandContext& context)
{
  AXmlElement& website = context.useModel().useRoot().addElement("website");
  AXmlElement& data = context.useModel().useRoot().addElement("data");

  AString str;
  AFilename relativePath;
  if (context.useRequestHeader().useUrl().useParameterPairs().get("path", str))
  {
    relativePath.set(str, true);
    if (!relativePath.compactPath())
    {
      relativePath.clear();
      str.clear();
    }
  }
  
  data.addElement("base", relativePath);
  data.addElement("root").addData(m_Services.useConfiguration().getBaseDir()); 

  AString locale;
  context.useRequestHeader().useUrl().useParameterPairs().get("locale", locale);
  if (!locale.isEmpty())
    context.useRequestHeader().set(AHTTPHeader::HT_REQ_Accept_Language, locale);

  _buildWebsiteXml(context, website, relativePath);

  // Current locale and available locales
  data.addElement("locale").addData(locale);
  AXmlElement& eAvail = data.addElement("select_locale").addElement("select").addAttribute("name", "locale");

  bool hasSelected = false;
  AXmlElement& eDefault = eAvail.addElement("option").addData(m_Services.useConfiguration().getBaseLocale()).addAttribute("base", "true");
  for (AOSConfiguration::MAP_LOCALE_DIRS::const_iterator cit = m_Services.useConfiguration().getLocaleStaticDirs().begin(); cit != m_Services.useConfiguration().getLocaleStaticDirs().end(); ++cit)
  {
    AXmlElement& eBase = eAvail.addElement("option").addData(cit->first);
    if (cit->first.equals(locale))
    {
      eBase.addAttribute("selected", "true");
      hasSelected = true;
    }
  }
  if (!hasSelected)
    eDefault.addAttribute("selected", "true");
}

void AOSAdminCommand_website_list::_insertStylesheet(AOSAdminCommandContext& context)
{
  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), ASW("/xsl/_command/website_list.xsl",30));
}

struct _WebSiteFilename
{
  enum TYPE
  {
    TYPE_UNKNOWN = 0,
    DIR_STATIC   = 1,
    DIR_DYNAMIC  = 2,    
    FILE_STATIC  = 3,
    FILE_DYNAMIC = 4
  };

  _WebSiteFilename() : type(TYPE_UNKNOWN)
  {
  }

  _WebSiteFilename(const AFilename& f, _WebSiteFilename::TYPE t, const AString& i = AConstant::ASTRING_EMPTY) :
    type(t),
    filename(f),
    info(i)
  {
  }
  
  _WebSiteFilename(const _WebSiteFilename& that) :
    type(that.type),
    filename(that.filename),
    info(that.info)
  {
  }
    
  AFilename filename;
  TYPE type;
  AString info;
};

struct _WebSiteInfo
{
  _WebSiteInfo() :
    pController(NULL),
    pDirConfig(NULL),
    type(0)
  {
  }

  _WebSiteInfo(const _WebSiteInfo& that) :
    pController(that.pController),
    pDirConfig(that.pDirConfig),
    type(that.type)
  {
  }

  const AOSController *pController;
  const AOSDirectoryConfig *pDirConfig;
  u4 type;
  
  typedef std::list<_WebSiteFilename> CONTAINER;
  CONTAINER paths;
};

typedef std::map<AString, _WebSiteInfo> WEBSITE_CONTAINER;

void AOSAdminCommand_website_list::_buildWebsiteXml(AOSAdminCommandContext& context, AXmlElement& node, AFilename& relativePath)
{
  // 0000 - Invalid
  // 0001 - static dir(1), 0010 - dynamic dir(2), 0011 - dyamic and static
  // 0100 - static(4), 1000 - dynamic(8), 1100 dynamic over static(12)
  WEBSITE_CONTAINER content;
  
  LIST_AFilename dirs;
  m_Services.useConfiguration().getAosStaticDirectoryChain(context.useRequestHeader(), dirs);
  for (LIST_AFilename::reverse_iterator itDir = dirs.rbegin(); itDir != dirs.rend(); ++itDir)
  {
    itDir->join(relativePath, true);
    
    //Check static
    AFileSystem::FileInfos files;
    if (AFileSystem::exists(*itDir) && AFileSystem::dir(*itDir, files, false, false))
    {
      for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
      {
        if (it->isDirectory())
        {
          // Don't add a new entry if it already exists
          it->filename.removeBasePath(*itDir);
          const AString& folderName = it->filename.usePathNames().back();
          _WebSiteInfo& info = content[folderName];

          _WebSiteFilename wsf;
          wsf.filename = it->filename;
          wsf.type = _WebSiteFilename::DIR_STATIC;
          it->filename.emitPath(wsf.info);
          info.paths.push_back(wsf);
          info.type = 1;
        }
        else
        {
          it->filename.removeBasePath(m_Services.useConfiguration().getBaseWebSiteDir());
          _WebSiteInfo& info = content[it->filename.useFilename()];

          info.type = 0x4;
          
          _WebSiteFilename wsf;
          wsf.filename = it->filename;
          wsf.type = _WebSiteFilename::FILE_STATIC;
          it->filename.emitPath(wsf.info);
          info.paths.push_back(wsf);
        }
      }
    }
  }

  AFilename dynamicPath(m_Services.useConfiguration().getAosBaseDynamicDirectory());
  dynamicPath.join(relativePath, true);

  AFileSystem::FileInfos files;
  if (AFileSystem::exists(dynamicPath) && AFileSystem::dir(dynamicPath, files, false, false))
  {
    for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->isDirectory())
      {
        it->filename.removeBasePath(dynamicPath);
        const AString& folderName = it->filename.usePathNames().back();
        _WebSiteInfo& info = content[folderName];

        // Don't add a new entry for dynamic if it already exists
        _WebSiteFilename wsf;
        wsf.filename = it->filename;
        wsf.type = _WebSiteFilename::DIR_DYNAMIC;
        it->filename.emitPath(wsf.info);
        info.paths.push_back(wsf);
        info.type |= 0x2;
      }
      else
      {
        it->filename.removeBasePath(m_Services.useConfiguration().getBaseWebSiteDir());
        if (it->filename.equalsExtension("aos","xml"))
        {
          AFilename f(it->filename);
          f.useFilename().rremove(8);  // remove ".aos.xml"
          _WebSiteInfo& info = content[f.useFilename()];
          info.type |= 0x8;
          
          _WebSiteFilename wsf;
          wsf.filename = it->filename;
          wsf.type = _WebSiteFilename::FILE_DYNAMIC;
          it->filename.emitPath(wsf.info);
          info.paths.push_back(wsf);

          // Lookup controller
          f.usePathNames().pop_front();
          f.makeAbsolute();
          
          if (f.useFilename().equals("__this__"))
            info.pDirConfig = m_Services.useConfiguration().getDirectoryConfig(f);
          else
            info.pController = m_Services.useConfiguration().getController(f);
        }
        else
        {
          _WebSiteInfo& info = content[it->filename.useFilename()];
          info.type = 0;
          info.paths.push_back(_WebSiteFilename(it->filename, _WebSiteFilename::TYPE_UNKNOWN));
        }
      }
    }
  }

  for (WEBSITE_CONTAINER::iterator it = content.begin(); it != content.end(); ++it)
  {
    switch(it->second.type)
    {
      case 1: // dir: static
      case 2: // dir: dynamic
      case 3: // dir: static+dynamic
      {
        AXmlElement& n = node.addElement("dir").addAttribute("type", it->second.type);
        n.addElement("name", it->first);
        for (_WebSiteInfo::CONTAINER::reverse_iterator itPath = it->second.paths.rbegin(); itPath != it->second.paths.rend(); ++itPath)
        {
          AXmlElement& p = n.addElement("path");
          p.addAttribute("type", (u4)itPath->type).addElement("data").addData(itPath->filename);
          p.addElement("info").addData(itPath->info);
        }
      }
      break;

      case 4: // file: static
      {
        AXmlElement& n = node.addElement("file").addAttribute("type", it->second.type);
        n.addElement("name", it->first);
        for (_WebSiteInfo::CONTAINER::reverse_iterator itPath = it->second.paths.rbegin(); itPath != it->second.paths.rend(); ++itPath)
        {
          AXmlElement& p = n.addElement("path");
          p.addAttribute("type", (u4)itPath->type).addElement("data").addData(itPath->filename);
          p.addElement("info").addData(itPath->info);
        }
      }
      break;

      case 8: // dynamic
      case 12: // both
      {
        AXmlElement& n = node.addElement("file").addAttribute("type", it->second.type);
        n.addElement("name", it->first);
        for (_WebSiteInfo::CONTAINER::iterator itPath = it->second.paths.begin(); itPath != it->second.paths.end(); ++itPath)
        {
          AXmlElement *p;
          if (it->second.pController->isEnabled())
            p = &n.addElement("path", true);
          else
            p = &n.addElement("path");

          p->addAttribute("type", (u4)itPath->type).addElement("data").addData(itPath->filename);
          p->addElement("info").addData(itPath->info);

          if ((itPath->type & _WebSiteFilename::FILE_DYNAMIC) && it->second.pController)
            it->second.pController->emitXml(p->addElement(AOSController::ELEMENT));
          if (it->second.pDirConfig)
            it->second.pDirConfig->emitXml(p->addElement(AOSDirectoryConfig::ELEMENT));
        }
      }
      break;

      default:
        node.addElement("file").addAttribute("type", it->second.type).addElement("name", it->first);
    }
  }
}
