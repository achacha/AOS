/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSConfiguration.hpp"
#include "AOSController.hpp"
#include "AFile_Physical.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AUrl.hpp"
#include "ALog.hpp"
#include "AFileSystem.hpp"
#include "AOSServices.hpp"
#include "AINIProfile.hpp"

const AString AOSConfiguration::CLASS("AOSConfiguration");

const AString AOSConfiguration::DATABASE_URL("/config/server/database/url");
const AString AOSConfiguration::DATABASE_CONNECTIONS("/config/server/database/connections");

int AOSConfiguration::FIRST_CHAR_RETRIES(15);
int AOSConfiguration::SLEEP_STARTTIME(30);
int AOSConfiguration::SLEEP_INCREMENT(20);
bool AOSConfiguration::IS_HTTP_PIPELINING_ENABLED(true);
bool AOSConfiguration::GZIP_IS_ENABLED(true);
size_t AOSConfiguration::GZIP_MIN_SIZE(10240);
int AOSConfiguration::GZIP_DEFAULT_LEVEL(6);
int AOSConfiguration::UNAVAILABLE_SLEEP_TIME(50);
int AOSConfiguration::UNAVAILABLE_RETRIES(100);

#define DEFAULT_AOSCONTEXTMANAGER_HISTORY_MAX_SIZE 100

void AOSConfiguration::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_AosBaseConfigDir=" << m_AosBaseConfigDir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosBaseStaticDir=" << m_AosBaseStaticDir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosBaseDynamicDir=" << m_AosBaseDynamicDir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosBaseDataDir=" << m_AosBaseDataDir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosDefaultFilename=" << m_AosDefaultFilename << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosDefaultInputProcessor=" << m_AosDefaultInputProcessor << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AosDefaultOutptGenerator=" << m_AosDefaultOutputGenerator << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AdminBaseHttpDir=" << m_AdminBaseHttpDir << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Config={" << std::endl;
  m_Config.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ConfigBits=" << std::endl;
  m_ConfigBits.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ReportedServer="  << m_ReportedServer << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHostname="  << m_ReportedHostname << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHttpPort="  << m_ReportedHttpPort << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHttpsPort=" << m_ReportedHttpsPort << std::endl;

  {
    // Controllers
    ADebugDumpable::indent(os, indent+1) << "m_ControllerPtrs={" << std::endl;
    for (MAP_ASTRING_CONTROLLERPTR::const_iterator cit = m_ControllerPtrs.begin(); cit != m_ControllerPtrs.end(); ++cit)
    {
      ADebugDumpable::indent(os, indent+2) << (*cit).first << "={" << std::endl;
      (*cit).second->debugDump(os, indent+3);
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

  {
    // Directory configs
    ADebugDumpable::indent(os, indent+1) << "m_DirectoryConfigs={" << std::endl;
    MAP_ASTRING_CONFIG::const_iterator cit = m_DirectoryConfigs.begin();
    while (cit != m_DirectoryConfigs.end())
    {
      ADebugDumpable::indent(os, indent+2) << (*cit).first << "={" << std::endl;
      (*cit).second->debugDump(os, indent+3);
      ++cit;
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

  ADebugDumpable::indent(os, indent+1) << "m_LocaleDataDirs={" << std::endl;
  for (AOSConfiguration::MAP_LOCALE_DIRS::const_iterator cit = m_LocaleDataDirs.begin(); cit != m_LocaleDataDirs.end(); ++cit)
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=" << (*cit).second.dir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_LocaleStaticDirs={" << std::endl;
  for (AOSConfiguration::MAP_LOCALE_DIRS::const_iterator cit = m_LocaleStaticDirs.begin(); cit != m_LocaleDataDirs.end(); ++cit)
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=" << (*cit).second.dir << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_LocaleRemap={" << std::endl;
  for (MAP_AString_AString::const_iterator cit = m_LocaleRemap.begin(); cit != m_LocaleRemap.end(); ++cit)
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=" << (*cit).second << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSConfiguration::getClass() const
{
  return CLASS;
}

void AOSConfiguration::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);
  
  adminAddProperty(eBase, ASWNL("reported server")    , m_ReportedServer);
  adminAddProperty(eBase, ASWNL("reported hostname")  , m_ReportedHostname);
  adminAddProperty(eBase, ASWNL("reported http port") , AString::fromInt(m_ReportedHttpPort));
  adminAddProperty(eBase, ASWNL("reported https port"), AString::fromInt(m_ReportedHttpsPort));

  {
    LIST_AString moduleNames;
    ARope rope;
    if (m_Services.getModules().getModuleNames(moduleNames) > 0)
    {
      LIST_AString::const_iterator cit = moduleNames.begin();
      while (cit != moduleNames.end())
      {
        if (cit != moduleNames.begin())
          rope.append(',');
        rope.append(*cit);
        ++cit;
      }
      adminAddProperty(
        eBase,
        ASWNL("dynamic_module_libraries"),
        rope
      );
    }
  }

  {
    AXmlElement& e = eBase.addElement("object").addAttribute("name","default");
    adminAddProperty(e, ASWNL("base static directory (for default locale)"), m_AosBaseStaticDir);
    adminAddProperty(e, ASWNL("base data directory (for default locale)"), m_AosBaseDataDir);
    adminAddProperty(e, ASWNL("base config directory"), m_AosBaseConfigDir);
    adminAddProperty(e, ASWNL("base dynamic directory"), m_AosBaseDynamicDir);
    adminAddProperty(e, ASWNL("base admin directory"), m_AdminBaseHttpDir);
    adminAddProperty(e, ASWNL("filname"), m_AosDefaultFilename);
    adminAddProperty(e, ASWNL("input processor"), m_AosDefaultInputProcessor);
    adminAddProperty(e, ASWNL("output generator"), m_AosDefaultOutputGenerator);
  }

  // Locale
  {
    AXmlElement& e = eBase.addElement("object").addAttribute("name","locale");
    adminAddProperty(e, ASWNL("default"), m_BaseLocale);

    ARope rope;
    for (AOSConfiguration::MAP_LOCALE_DIRS::const_iterator cit = m_LocaleDataDirs.begin(); cit != m_LocaleDataDirs.end(); ++cit)
    {  
      rope.append((*cit).first);
      rope.append('=');
      rope.append((*cit).second.dir);
      rope.append(AConstant::ASTRING_CRLF);
    }
    adminAddProperty(e, ASWNL("data"), rope);

    rope.clear();
    for (AOSConfiguration::MAP_LOCALE_DIRS::const_iterator cit = m_LocaleStaticDirs.begin(); cit != m_LocaleStaticDirs.end(); ++cit)
    {  
      rope.append((*cit).first);
      rope.append('=');
      rope.append((*cit).second.dir);
      rope.append(AConstant::ASTRING_CRLF);
    }
    adminAddProperty(e, ASWNL("static"), rope);

    rope.clear();
    for (MAP_AString_AString::const_iterator cit = m_LocaleRemap.begin(); cit != m_LocaleRemap.end(); ++cit)
    {  
      rope.append((*cit).first);
      rope.append('=');
      rope.append((*cit).second);
      rope.append(AConstant::ASTRING_CRLF);
    }
    adminAddProperty(e, ASWNL("remap"), rope);
  }

  // Controllers
  {
    ARope rope;
    for (MAP_ASTRING_CONTROLLERPTR::const_iterator cit = m_ControllerPtrs.begin(); cit != m_ControllerPtrs.end(); ++cit)
    {
      rope.append((*cit).second->getPath());
      if (!(*cit).second->getAlias().isEmpty())
      {
        rope.append("(",1);
        rope.append((*cit).second->getAlias());
        rope.append(")",1);
      }
      rope.append(AConstant::ASTRING_CRLF);
    }
    adminAddProperty(eBase, ASW("Controllers",11), rope);
  }

  {
    ARope rope;
    m_Config.emit(rope, 0);
    AXmlElement& eObject = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("XMLConfig",9));
    adminAddProperty(
      eObject,
      ASW("config",6),
      rope,
      AXmlElement::ENC_CDATASAFE
    );
  }
}

void AOSConfiguration::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("display_name",12), str))
  {
    MAP_ASTRING_CONTROLLERPTR::iterator it = m_ControllerPtrs.find(str);
    if (it != m_ControllerPtrs.end())
      (*it).second->adminProcessAction(eBase, request);
    else
      adminAddError(eBase, "command not found");
  }
  else
  {
    adminAddError(eBase, "'display_name' input not found");
    return;
  }
}

AOSConfiguration::AOSConfiguration(
  const AFilename& baseDir,
  AOSServices& services
) :
  m_BaseDir(baseDir),
  m_AosBaseConfigDir(baseDir),
  m_AosBaseDataDir(baseDir),
  m_AosBaseStaticDir(baseDir),
  m_AosBaseDynamicDir(baseDir),
  m_AdminBaseHttpDir(baseDir),
  m_Services(services),
  m_Config(ASW("config",6)),
  m_ReportedServer(AOS_SERVER_NAME),
  m_ReportedHostname("localhost",9),
  m_ReportedHttpPort(80),
  m_ReportedHttpsPort(443)
{
  adminRegisterObject(services.useAdminRegistry());

  // Config directory
  m_AosBaseConfigDir.usePathNames().push_back("conf");

  // Admin 
  m_AdminBaseHttpDir.usePathNames().push_back("admin");

  // Read the config
  AFilename filename(m_AosBaseConfigDir);
  filename.useFilename().assign("AObjectServer.xml",17);
  loadConfig(filename);

  const AString& websiteDirectory = m_Config.useRoot().getString("/config/server/website-directory", AConstant::ASTRING_EMPTY);
  if (!websiteDirectory.isEmpty())
  {
    m_AosBaseDynamicDir.usePathNames().push_back(websiteDirectory);
    m_AosBaseDataDir.usePathNames().push_back(websiteDirectory);
    m_AosBaseStaticDir.usePathNames().push_back(websiteDirectory);
  }

  m_AosBaseDynamicDir.usePathNames().push_back("dynamic");
  m_AosBaseDataDir.usePathNames().push_back("data");
  m_AosBaseStaticDir.usePathNames().push_back("static");
  
  // Get compressed extensions
  _populateGzipCompressionExtensions();

  // Read in I18N directories
  _loadLocaleInfo();
  _populateLocaleDirectories(m_AosBaseStaticDir, m_LocaleStaticDirs);
  _populateLocaleDirectories(m_AosBaseStaticDir, m_LocaleDataDirs);

  // Configure server internals from config
  setAosDefaultFilename("/config/server/default-filename");
  setAosDefaultInputProcessor("/config/server/default-input-processor");
  setAosDefaultOutputGenerator("/config/server/default-output-generator");

  // Configure server reported values
  setReportedServer("/config/server/reported/server");
  setReportedHostname("/config/server/reported/hostname");
  setReportedHttpPort("/config/server/reported/http");
  setReportedHttpsPort("/config/server/reported/https");

  // Read string data
  m_DefaultMimeType = m_Config.useRoot().getString("/config/server/default-mime-type", ASW("text/xml; charset=utf-8",23));

  // Statics
  _initStatics();

  int logLevel = m_Config.useRoot().getInt("/config/server/log-level", -1);
  if (-1 != logLevel)
  {
    m_Services.useLog().setEventMask(ALog::EVENTMASK_SILENT);
    switch(logLevel)
    {
      case 5 : m_Services.useLog().addEventMask(ALog::EVENTMASK_ALL);
        break;

      case 4 : m_Services.useLog().addEventMask(ALog::EVENTMASK_ALL_INFO);
      case 3 : m_Services.useLog().addEventMask(ALog::EVENTMASK_ALL_MESSAGES);
      case 2 : m_Services.useLog().addEventMask(ALog::EVENTMASK_ALL_WARNINGS);
      case 1 : m_Services.useLog().addEventMask(ALog::EVENTMASK_ALL_ERRORS);
        break;

      default: m_Services.useLog().setEventMask(ALog::EVENTMASK_DEFAULT);
    }
  }

  m_IsOverrideInputAllowed = m_Config.getRoot().getBool("/config/server/debug/allow-overrideInput", false);
  m_IsOverrideOutputAllowed = m_Config.getRoot().getBool("/config/server/debug/allow-overrideOutput", false);
  m_IsDumpContextAllowed = m_Config.getRoot().getBool("/config/server/debug/allow-dumpContext", false);

  try
  {
    _readMIMETypes();
  }
  catch(AException& ex)
  {
    // Log any exception and re-throw since we just created the log
    services.useLog().addException(ex);
    throw;
  }
}

void AOSConfiguration::loadControllers()
{
  try
  {
    AFileSystem::FileInfos fileList;
    AFileSystem::FileInfos directoryConfigs;
    const AString& EXT1 = ASW("aos",3);
    const AString& EXT2 = ASW("xml",3);
    if (AFileSystem::dir(m_AosBaseDynamicDir, fileList, true, true) > 0)
    {
      for(AFileSystem::FileInfos::iterator it = fileList.begin(); it != fileList.end(); ++it)
      {
        if (!it->filename.equalsExtension(EXT1, EXT2))
        {
          continue;
        }
        
        if (it->filename.useFilename().equals(AOSDirectoryConfig::FILENAME))
        {
          _readDirectoryConfig(it->filename);
        }
        else
        {
          _readController(it->filename);
        }
      }

      _postProcessControllerAndConfig(directoryConfigs);
    }
    else
    {
      AString str("WARNING: No command files found at: '");
      str.append(m_AosBaseDynamicDir);
      str.append("'");
      AOS_DEBUGTRACE(str.c_str(), NULL);
      m_Services.useLog().add(str, m_AosBaseDynamicDir, ALog::EVENT_WARNING);
    }
  }
  catch(AException& ex)
  {
    // Log any exception and re-throw since we just created the log
    m_Services.useLog().addException(ex);
    throw;
  }
}
  
bool AOSConfiguration::validateControllers()
{
  bool status = true;
  AOSInputExecutor& inputExecutor = m_Services.useInputExecutor();
  AOSModuleExecutor& moduleExecutor = m_Services.useModuleExecutor();
  AOSOutputExecutor& outputExecutor = m_Services.useOutputExecutor();
  for(MAP_ASTRING_CONTROLLERPTR::iterator it = m_ControllerPtrs.begin(); it != m_ControllerPtrs.end(); ++it)
  {
    // Input
    {
      const AString& name = it->second->getInputProcessorClassName();
      if (!name.isEmpty() && !inputExecutor.exists(name))
      {
        // Input processor does not exist
        AString strError("Input processor not found (");
        strError.append(name);
        strError.append(") in controller: ");
        strError.append(it->first);
        strError.append(AConstant::ASTRING_EOL);
        strError.append(*(it->second));
        m_Services.useLog().add(strError, ALog::EVENT_FAILURE);
        status = false;
      }
    }

    // Modules
    {
      const AOSModules::LIST_AOSMODULE_PTRS& modules = it->second->getModules().get();
      for (AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = modules.begin(); cit != modules.end(); ++cit)
      {
        const AString& name = (*cit)->getModuleClassName();
        AASSERT(it->second, !name.isEmpty());
        if (!moduleExecutor.exists(name))
        {
          // Input processor does not exist
          AString strError("Module not found (");
          strError.append(name);
          strError.append(") in controller: ");
          strError.append(it->first);
          strError.append(AConstant::ASTRING_EOL);
          strError.append(*(it->second));
          m_Services.useLog().add(strError, ALog::EVENT_FAILURE);
          status = false;
        }
      }
    }


    // Output
    {
      const AString& name = it->second->getOutputGeneratorClassName();
      if (!name.isEmpty() && !outputExecutor.exists(name))
      {
        // Input processor does not exist
        AString strError("Output generator not found (");
        strError.append(name);
        strError.append(") in controller: ");
        strError.append(it->first);
        strError.append(AConstant::ASTRING_EOL);
        strError.append(*(it->second));
        m_Services.useLog().add(strError, ALog::EVENT_FAILURE);
        status = false;
      }
    }
  }
  
  return status;
}

void AOSConfiguration::_initStatics()
{
  FIRST_CHAR_RETRIES = m_Config.useRoot().getInt(ASWNL("/config/server/http/first-char-read-tries"), FIRST_CHAR_RETRIES);
  SLEEP_STARTTIME = m_Config.useRoot().getInt(ASWNL("/config/server/http/first-char-sleep-start"), SLEEP_STARTTIME);
  SLEEP_INCREMENT = m_Config.useRoot().getInt(ASWNL("/config/server/http/first-char-sleep-increment"), SLEEP_INCREMENT);
  UNAVAILABLE_SLEEP_TIME = m_Config.useRoot().getInt(ASWNL("/config/server/http/unavailable-sleep-time"), UNAVAILABLE_SLEEP_TIME);
  UNAVAILABLE_RETRIES = m_Config.useRoot().getInt(ASWNL("/config/server/http/unavailable-retries"), UNAVAILABLE_RETRIES);
  IS_HTTP_PIPELINING_ENABLED = m_Config.useRoot().getBool(ASWNL("/config/server/http/http11-pipelining-enabled"), IS_HTTP_PIPELINING_ENABLED);
  GZIP_IS_ENABLED = m_Config.useRoot().getBool(ASWNL("/config/server/gzip-compression/enabled"), GZIP_IS_ENABLED);
  GZIP_MIN_SIZE = m_Config.useRoot().getSize_t(ASWNL("/config/server/gzip-compression/minimum-threshold"), GZIP_MIN_SIZE);
  GZIP_DEFAULT_LEVEL = m_Config.useRoot().getInt(ASWNL("/config/server/gzip-compression/default-level"), GZIP_DEFAULT_LEVEL);
}

void AOSConfiguration::_loadLocaleInfo()
{
  // Get default language
  m_BaseLocale = m_Config.useRoot().getString("/config/server/locale/base", ASW("en-us",5));
  m_Services.useLog().add(AString("Base locale: ")+m_BaseLocale);

  AXmlElement::CONST_CONTAINER langNodes;
  if (m_Config.useRoot().find("/config/server/locale/remap/lang", langNodes) > 0)
  {
    AString name;
    AString locale;
    for (AXmlElement::CONST_CONTAINER::iterator it = langNodes.begin(); it != langNodes.end(); ++it)
    {
      if (!(*it)->getAttributes().get(ASW("name",4), name))
        ATHROW(this, AException::InvalidConfiguration);  // MUST have name for each lang

      (*it)->emitContent(locale);
      if (locale.isEmpty())
        locale.assign(m_BaseLocale);
      
      name.makeLower();
      locale.makeLower();
      m_LocaleRemap[name] = locale;
      name.clear();
      locale.clear();
    }
  }
}

void AOSConfiguration::_populateLocaleDirectories(const AFilename& basedir, AOSConfiguration::MAP_LOCALE_DIRS& dirs)
{
  AASSERT(this, basedir.getFilename().getSize() == 0);

  // Get all locale variants
  // static/ -> static.*/
  AFilename f(basedir);
  
  // pathname of the last directory becomes part of the wildcard filename to search on
  f.useFilename().assign(f.usePathNames().back());
  f.usePathNames().pop_back();
  f.useFilename().append(".*",2);
  if (!AFileSystem::existsExpandWildcards(f))
  {
    m_Services.useLog().add(AString("Assuming single locale, locale config directory not found: ")+f, ALog::EVENT_INFO);
    return;
  }

  AFileSystem::FileInfos fileInfos;
  if (AFileSystem::dir(f, fileInfos, false, false) > 0)
  {
    // Get all locale based directories
    for (AFileSystem::FileInfos::iterator it = fileInfos.begin(); it != fileInfos.end(); ++it)
    {
      if (it->typemask & AFileSystem::Directory)
      {
        AString lang;
        AString& last = it->filename.usePathNames().back();
        size_t pos = last.rfind('.');
        if (AConstant::npos != pos)
        {
          // default language
          last.peek(lang, pos+1);
          AASSERT_EX(&(*it), dirs.find(lang) == dirs.end(), AString("Duplicate language directory: ")+it->filename);
          lang.makeLower();
          dirs[lang] = LocaleDirInfo(it->filename);
        }
      }
    }
  }

  // Link languages to parent (en-gb to en)
  for (AOSConfiguration::MAP_LOCALE_DIRS::iterator it = dirs.begin(); it != dirs.end(); ++it)
  {
    AString lang(it->first);
    lang.rremoveUntil('-');
    if (!lang.isEmpty())
    {
      AOSConfiguration::MAP_LOCALE_DIRS::iterator itLang = dirs.find(lang);
      if (itLang != dirs.end())
      {
        // parent language exists
        it->second.setPrev(&(itLang->second));
      }
    }
  }

  // Add entries for remapped locales
  for (MAP_AString_AString::iterator itRemap = m_LocaleRemap.begin(); itRemap != m_LocaleRemap.end(); ++itRemap)
  {
    // Check if the target locale exists
    AOSConfiguration::MAP_LOCALE_DIRS::iterator itRemappedLocalInfo = dirs.find(itRemap->second);
    if (itRemappedLocalInfo == dirs.end())
    {
      m_Services.useLog().add(AString("Physical locale directory for remap does not exist (ignoring and using default): ")+itRemap->second, ALog::EVENT_WARNING);
      continue;
    }
    
    // Warn if remapping an existing directory
    if (dirs.find(itRemap->first) != dirs.end())
    {
      m_Services.useLog().add(AString("Remapping locale replaces an existing directory: ")+itRemap->first+ASWNL(" to ")+itRemap->second, ALog::EVENT_WARNING);
    }

    // Remap locale to a directory of an existing locale
    dirs[itRemap->first] = itRemappedLocalInfo->second;
  }
}

AOSConfiguration::~AOSConfiguration()
{
  try
  {
    {
      MAP_ASTRING_CONTROLLERPTR::iterator it = m_ControllerPtrs.begin();
      while (it != m_ControllerPtrs.end())
      {
        delete it->second;
        ++it;
      }
    }
    {
      MAP_ASTRING_CONFIG::iterator it = m_DirectoryConfigs.begin();
      while (it != m_DirectoryConfigs.end())
      {
        delete it->second;
        ++it;
      }
    }
  }
  catch(...) {}
}

void AOSConfiguration::_readMIMETypes()
{
  if (m_Config.useRoot().exists(ASWNL("/config/server/mime-types")))
  {
    AString str;
    m_Config.useRoot().emitString(ASWNL("/config/server/mime-types"), str);

    AFilename filename(m_AosBaseConfigDir);
    AFilename fMime(str, false);
    filename.join(fMime);

    AINIProfile mimeTypes(filename);
    AINIProfile::Iterator it = mimeTypes.getIterator(ASWNL("MIME_Types"));
    AString strName(16,16), strValue(64, 16);
    while (it.next())
    {
      it.getName(strName);
      it.getValue(strValue);
      if (!strName.isEmpty())
      {
        strName.makeLower();
        if (m_ExtToMimeType.find(strName) == m_ExtToMimeType.end())
        {
          m_ExtToMimeType[strName] = strValue;
          // Creates too much spam on startup m_Services.useLog().add(ASWNL("Adding MIME type"), strName, strValue, ALog::EVENT_INFO);
        }
        else
        {
          m_Services.useLog().add(ASWNL("Ignoring duplicate MIME type"), strName, strValue, filename, ALog::EVENT_WARNING);
        }
      }
    }
  }
}

const AFilename& AOSConfiguration::getBaseDir() const
{
  return m_BaseDir;
}

const AString& AOSConfiguration::getBaseLocale() const
{
  return m_BaseLocale;
}

bool AOSConfiguration::getMimeTypeFromExt(const AString& ext, AString& target) const
{
  // No extension, no mapping
  if (ext.isEmpty())
    return false;

  AString strExt(ext);
  strExt.makeLower();
  MAP_AString_AString::const_iterator cit = m_ExtToMimeType.find(strExt);
  if (cit != m_ExtToMimeType.end())
  {
    target.assign(cit->second);
    return true;
  }
  else
    return false;
}

const AString& AOSConfiguration::getDefaultMimeType() const
{
  return m_DefaultMimeType;
}

void AOSConfiguration::setMimeTypeFromExt(const AString& ext, AOSContext& context)
{
  AString mimetype;
  if (getMimeTypeFromExt(ext, mimetype))
  {
    context.useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, mimetype);
  }
  else
  {
    context.useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, m_DefaultMimeType);
  }
}

void AOSConfiguration::setMimeTypeFromRequestExt(AOSContext& context)
{
  setMimeTypeFromExt(context.useRequestUrl().getExtension(), context);
}

void AOSConfiguration::loadConfig(const AFilename& filename)
{
  if (AFileSystem::exists(filename))
  {
    m_Services.useLog().add(ASW("Reading XML config: ",20), filename, ALog::EVENT_DEBUG);
    
    AFile_Physical file(filename);
    file.open();
    AXmlDocument configDoc(file);
    m_Config.useRoot().addContent(configDoc.useRoot());
  }
  else
    m_Services.useLog().add(ASW("XML config does not exist: ",27), filename, ALog::EVENT_WARNING);
}

void AOSConfiguration::_readDirectoryConfig(AFilename& filename)
{
  // Open config file before mangling the path
  AFile_Physical configFile(filename);
  AString strPath(1536, 512);

  // Add directory config to map
  filename.removeBasePath(m_AosBaseDynamicDir, true);   // Remove dynamic dir
  filename.emitPath(strPath);                           // Path only for map key
  AASSERT_EX(this, m_DirectoryConfigs.find(strPath) == m_DirectoryConfigs.end(), filename);
  
  {
    AString str("Reading directory config file: ");
    str.append(strPath);
    AOS_DEBUGTRACE(str.c_str(), NULL);
  }  

  // Parse XML document
  configFile.open();
  AAutoPtr<AXmlDocument> pDoc(new AXmlDocument(configFile), true);
  configFile.close();
  AASSERT_EX(pDoc, pDoc->useRoot().getName().equals(AOSDirectoryConfig::ELEMENT), filename);
  
  // Add directory config
  AOSDirectoryConfig *p =  new AOSDirectoryConfig(strPath, pDoc->useRoot(), m_Services.useLog());
  m_DirectoryConfigs[strPath] = p;

  // Register the command with admin
  p->adminRegisterObject(m_Services.useAdminRegistry());
}

void AOSConfiguration::_readController(AFilename& filename)
{
  // Load from all XML command files
  AFile_Physical commandFile(filename);
  commandFile.open();
  AXmlDocument doc(commandFile);
  commandFile.close();

  AXmlElement::CONST_CONTAINER nodes;
  doc.useRoot().find(AOSController::S_CONTROLLER_ROOT, nodes);
  
  // Parse each /command/command type
  AString strPath(1536, 512);
  AString dynamicDir(m_AosBaseDynamicDir.toAString());
  AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin();
  while(cit != nodes.end())
  {
    const AXmlElement *pElement = dynamic_cast<const AXmlElement *>(*cit);
    if (pElement)
    {
      // Get relative path
      filename.emit(strPath);
      AASSERT(this, strPath.getSize() >= dynamicDir.getSize());
      strPath.remove(dynamicDir.getSize()-1);
      strPath.rremove(8);   // .aos.xml to be removed

      {
        AString str(strPath);
        str.append('=');
        filename.emit(str);
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }  

      // Parse command and associate to relative path
      AAutoPtr<AOSController> p(new AOSController(strPath, m_Services.useLog()), true);
      p->fromXml(*pElement);
      AASSERT(this, m_ControllerPtrs.end() == m_ControllerPtrs.find(strPath));
      m_ControllerPtrs[strPath] = p;
      p.setOwnership(false);

      // Register the command with admin
      p->adminRegisterObject(m_Services.useAdminRegistry());

      strPath.clear();
    }
    ++cit;
  }
}

void AOSConfiguration::_postProcessControllerAndConfig(AFileSystem::FileInfos& directoryConfigs)
{
  AString strPath(1536, 512);
  for (AFileSystem::FileInfos::iterator it = directoryConfigs.begin(); it != directoryConfigs.end(); ++it)
  {
    std::cout << "-=-=-=-=-=-=:directory config: " << it->filename << std::endl;
  }
}

const AOSController * const AOSConfiguration::getController(const AUrl& commandUrl) const
{
  AString command(commandUrl.getPathAndFilename());
  if (commandUrl.getFilename().isEmpty())
    command.append(m_AosDefaultFilename);

  MAP_ASTRING_CONTROLLERPTR::const_iterator cit = m_ControllerPtrs.find(command);
  if (cit != m_ControllerPtrs.end())
    return (*cit).second;
  else
    return NULL;
}

const AOSDirectoryConfig * const AOSConfiguration::getDirectoryConfig(const AUrl& commandUrl) const
{
  AString dir(commandUrl.getPath());
  while (dir.getSize() > 0)
  {
    MAP_ASTRING_CONFIG::const_iterator cit = m_DirectoryConfigs.find(dir);
    if (cit != m_DirectoryConfigs.end())
      return (*cit).second;
    else
    {
      if (AConstant::npos == dir.rremoveUntil('/', true) || AConstant::npos == dir.rremoveUntil('/', false))
        break;
    }
  }
  return NULL;
}

const AFilename& AOSConfiguration::getAosBaseConfigDirectory() const
{
  return m_AosBaseConfigDir;
}

const AFilename& AOSConfiguration::getAosBaseDynamicDirectory() const
{
  return m_AosBaseDynamicDir;
}

const AFilename& AOSConfiguration::getAosBaseStaticDirectory() const
{
  return m_AosBaseStaticDir;
}

const AFilename& AOSConfiguration::getAosBaseDataDirectory() const
{
  return m_AosBaseDataDir;
}

void AOSConfiguration::getAosStaticDirectory(AOSContext& context, AFilename& filename) const
{
  _getLocaleAosDirectory(context, filename, m_LocaleStaticDirs, m_AosBaseStaticDir);
}

void AOSConfiguration::getAosDataDirectory(AOSContext& context, AFilename& filename) const
{
  _getLocaleAosDirectory(context, filename, m_LocaleDataDirs, m_AosBaseDataDir);
}

void AOSConfiguration::_getLocaleAosDirectory(AOSContext& context, AFilename& filename, const AOSConfiguration::MAP_LOCALE_DIRS& dirs, const AFilename& defaultDir) const
{
  if (dirs.size() > 0)
  {
    LIST_AString langs;
    if (context.useRequestHeader().getAcceptLanguageList(langs) > 0)
    {
      for(LIST_AString::iterator it = langs.begin(); it != langs.end(); ++it)
      {
        // Check if default encoding is used
        if ((*it).equals(m_BaseLocale))
          break;
        
        // Wildcard, use default
        if ((*it).equals("*",1))
          break;
          
        AOSConfiguration::MAP_LOCALE_DIRS::const_iterator citLang = dirs.find(*it);
        if (dirs.end() != citLang)
        {
          // Found a match
          filename.set(citLang->second.dir);
          return;
        }
      }
    }
  }

  filename.set(defaultDir);
}

void AOSConfiguration::getAosStaticDirectoryChain(AOSContext& context, LIST_AFilename& directories)
{
  _getLocaleAosDirectoryChain(context, directories, m_LocaleStaticDirs);
  directories.push_back(m_AosBaseStaticDir);
}

void AOSConfiguration::getAosDataDirectoryChain(AOSContext& context, LIST_AFilename& directories)
{
  _getLocaleAosDirectoryChain(context, directories, m_LocaleDataDirs);
  directories.push_back(m_AosBaseDataDir);
}

void AOSConfiguration::_getLocaleAosDirectoryChain(AOSContext& context, LIST_AFilename& directories, const AOSConfiguration::MAP_LOCALE_DIRS& dirs) const
{
  if (dirs.size() > 0)
  {
    LIST_AString langs;
    if (context.useRequestHeader().getAcceptLanguageList(langs) > 0)
    {
      for(LIST_AString::iterator it = langs.begin(); it != langs.end(); ++it)
      {
        // Check if default encoding is used
        if ((*it).equals(m_BaseLocale))
          break;
        
        // Wildcard, use default
        if ((*it).equals("*",1))
          break;
          
        AOSConfiguration::MAP_LOCALE_DIRS::const_iterator citLang = dirs.find(*it);
        if (dirs.end() != citLang)
        {
          // Found a match
          directories.push_back(citLang->second.dir);
        }
      }
    }
  }
}

const AString& AOSConfiguration::getAosDefaultFilename() const
{
  return m_AosDefaultFilename;
}

const AFilename& AOSConfiguration::getAdminBaseHttpDir() const
{
  return m_AdminBaseHttpDir;
}

void AOSConfiguration::getDynamicModuleConfigsToLoad(AFileSystem::FileInfos& target) const
{
  AFilename configLoad(getAosBaseConfigDirectory());
  configLoad.usePathNames().push_back(ASW("autoload",8));
  configLoad.useFilename().assign("*.xml",5);
  AFileSystem::dir(configLoad, target);
}

int AOSConfiguration::getReportedHttpPort() const
{
  return m_ReportedHttpPort;
}

int AOSConfiguration::getReportedHttpsPort() const
{
  return m_ReportedHttpsPort;
}

const AString& AOSConfiguration::getReportedHostname() const
{
  return m_ReportedHostname;
}

const AString& AOSConfiguration::getReportedServer() const
{
  return m_ReportedServer;
}

void AOSConfiguration::setReportedServer(const AString& configPath)
{
  AString str;
  if (m_Config.useRoot().emitString(configPath, str))
  {
    if (!str.isEmpty())
    {
      m_ReportedServer.clear();
      m_Config.useRoot().emitString(configPath, m_ReportedServer);
      AString str("Setting reported server: ");
      str.append(m_ReportedServer);
      m_Services.useLog().add(str, ALog::EVENT_DEBUG);
    }
  }
  else
  {
    AString str("AOSConfiguration::setReportedServer: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

void AOSConfiguration::setReportedHostname(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    AString str;
    m_Config.useRoot().emitString(configPath, str);

    // If hostname exists, set, else use default ip
    if (!str.isEmpty())
    {
      m_ReportedHostname.assign(str);
      
      AString str("Setting reported hostname: ");
      str.append(m_ReportedHostname);
      m_Services.useLog().add(str, ALog::EVENT_DEBUG);
    }
    else
    {
      m_ReportedHostname.assign(ASocketLibrary::getDefaultIp());
    }
  }
  else
  {
    AString str("AOSConfiguration::setReportedHostname: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

void AOSConfiguration::setReportedHttpPort(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_ReportedHttpPort = m_Config.useRoot().getInt(configPath, 80);
    
    AString str("Setting reported http port: ");
    str.append(AString::fromInt(m_ReportedHttpPort));
    m_Services.useLog().add(str, ALog::EVENT_DEBUG);
  }
  else
  {
    AString str("AOSConfiguration::setReportedHttpPort: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

void AOSConfiguration::setReportedHttpsPort(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_ReportedHttpsPort = m_Config.useRoot().getInt(configPath, 443);
    
    AString str("Setting reported https port: ");
    str.append(AString::fromInt(m_ReportedHttpsPort));
    m_Services.useLog().add(str, ALog::EVENT_DEBUG);
  }
  else
  {
    AString str("AOSConfiguration::setReportedHttpsPort: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

void AOSConfiguration::setAosDefaultFilename(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultFilename);
    
    AString str("Setting default filename: ");
    str.append(m_AosDefaultFilename);
    m_Services.useLog().add(str, ALog::EVENT_DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultFilename: Unable to find path: ")+configPath, ALog::EVENT_WARNING);
}

void AOSConfiguration::setAosDefaultInputProcessor(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultInputProcessor);
    
    AString str("Setting default input processor: ");
    str.append(m_AosDefaultInputProcessor);
    m_Services.useLog().add(str, ALog::EVENT_DEBUG);
  }
  else
  {
    AString str("AOSConfiguration::setAosDefaultInputProcessor: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

void AOSConfiguration::setAosDefaultOutputGenerator(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultOutputGenerator);
    
    AString str("Setting default output generator: ");
    str.append(m_AosDefaultOutputGenerator);
    m_Services.useLog().add(str, ALog::EVENT_DEBUG);
  }
  else
  {
    AString str("AOSConfiguration::setAosDefaultOutputGenerator: Unable to find path: ");
    str.append(configPath);
    m_Services.useLog().add(str, ALog::EVENT_WARNING);
  }
}

const AString& AOSConfiguration::getAosDefaultInputProcessor() const
{
  return m_AosDefaultInputProcessor;
}

const AString& AOSConfiguration::getAosDefaultOutputGenerator() const
{
  return m_AosDefaultOutputGenerator;
}

const AXmlElement& AOSConfiguration::getConfigRoot() const
{
  return m_Config.getRoot();
}

AXmlElement& AOSConfiguration::useConfigRoot()
{
  return m_Config.useRoot();
}

bool AOSConfiguration::isDumpContextAllowed() const
{
  return m_IsDumpContextAllowed;
}

bool AOSConfiguration::isInputOverrideAllowed() const
{
  return m_IsOverrideInputAllowed;
}

bool AOSConfiguration::isOutputOverrideAllowed() const
{
  return m_IsOverrideOutputAllowed;
}

void AOSConfiguration::convertUrlToReportedServerAndPort(AUrl& url) const
{
  url.setServer(m_Services.useConfiguration().getReportedHostname());
  switch(url.getProtocolEnum())
  {
    case AUrl::HTTP:
      url.setPort(m_Services.useConfiguration().getReportedHttpPort());
    break;

    case AUrl::HTTPS:
      url.setPort(m_Services.useConfiguration().getReportedHttpsPort());
    break;
  }
}

const SET_AString& AOSConfiguration::getCompressedExtensions() const
{
  return m_CompressedExtensions;
}

void AOSConfiguration::_populateGzipCompressionExtensions()
{
  AXmlElement::CONST_CONTAINER result;
  m_Config.getRoot().find(ASW("/config/server/gzip-compression/extensions/ext",46), result);
  AString str;
  for(AXmlElement::CONST_CONTAINER::iterator it = result.begin(); it != result.end(); ++it)
  {
    (*it)->emitContent(str);
    m_CompressedExtensions.insert(str);
    str.clear();
  }
}
