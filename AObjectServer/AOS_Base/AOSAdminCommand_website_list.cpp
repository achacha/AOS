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
  AFilename basePath;
  if (context.useRequestHeader().useUrl().useParameterPairs().get("basepath", str))
  {
    basePath.set(str, false);
    if (!basePath.compactPath())
    {
      basePath.clear();
      str.clear();
    }
  }

  if (str.isEmpty())
    data.addElement("base").addData("/");
  else
    data.addElement("base").addData(basePath);
  
  AUrl url(context.useRequestHeader().useUrl());
  url.useParameterPairs().remove("basepath");
  data.addElement("url").addData(url, AXmlElement::ENC_CDATADIRECT);
  data.addAttribute("cwd", basePath);
  data.addElement("root").addData(m_Services.useConfiguration().getBaseDir()); 

  AString locale;
  context.useRequestHeader().useUrl().useParameterPairs().get("locale", locale);
  if (!locale.isEmpty())
    context.useRequestHeader().set(AHTTPHeader::HT_REQ_Accept_Language, locale);

  AFilename staticPath;
  m_Services.useConfiguration().getAosStaticDirectory(context.useRequestHeader(), staticPath);
  staticPath.join(basePath, true);

  AFilename dynamicPath(m_Services.useConfiguration().getAosBaseDynamicDirectory());
  dynamicPath.join(basePath, true);

  _buildWebsiteXml(website, staticPath, dynamicPath);

  data.addElement("locale").addData(locale);
}

void AOSAdminCommand_website_list::_insertStylesheet(AOSAdminCommandContext& context)
{
  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), ASW("/xsl/_command/website_list.xsl",30));
}

struct _WebSiteInfo
{
  _WebSiteInfo() :
    pController(NULL),
    type(0)
  {
  }

  _WebSiteInfo(const _WebSiteInfo& that) :
    pController(that.pController),
    type(that.type)
  {
  }

  AOSController *pController;
  u4 type;
};

void AOSAdminCommand_website_list::_buildWebsiteXml(AXmlElement& node, AFilename& staticPath, AFilename& dynamicPath)
{
  // 0000 - Invalid
  // 0001 - static dir(1), 0010 - dynamic dir(2), 0011 - dyamic and static
  // 0100 - static(4), 1000 - dynamic(8), 1100 dynamic over static(12)
  std::map<AString, _WebSiteInfo> content;
  
  //Check static
  AFileSystem::FileInfos files;
  if (AFileSystem::exists(staticPath) && AFileSystem::dir(staticPath, files, false, false))
  {
    for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->isDirectory())
      {
        content[it->filename.usePathNames().back()].type = 1;
      }
      else
      {
        content[it->filename.useFilename()].type = 0x4;
      }
    }
  }

  files.clear();
  if (AFileSystem::exists(dynamicPath) && AFileSystem::dir(dynamicPath, files, false, false))
  {
    for (AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->isDirectory())
      {
        content[it->filename.usePathNames().back()].type |= 0x2;
      }
      else
      {
        if (it->filename.equalsExtension("aos","xml"))
        {
          it->filename.useFilename().rremove(8);  // remove ".aos.xml"
          content[it->filename.useFilename()].type |= 0x8;
        }
        else
        {
          content[it->filename.useFilename()].type = -1;
        }
      }
    }
  }

  for (std::map<AString, _WebSiteInfo>::iterator it = content.begin(); it != content.end(); ++it)
  {
    switch(it->second.type)
    {
      case 1: // dir: static
      {
        node.addElement("dir").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      case 2: // dir: dynamic
      {
        node.addElement("dir").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      case 3: // dir: static+dynamic
      {
        node.addElement("dir").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      case 4: // file: static
      {
        node.addElement("file").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      case 8: // dynamic
      {
        node.addElement("file").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      case 12: // both
      {
        node.addElement("file").addData(it->first).addAttribute("type", it->second.type);
      }
      break;

      default:
        node.addElement("ERROR").addData(it->first).addAttribute("type", it->second.type);
    }
  }
}
