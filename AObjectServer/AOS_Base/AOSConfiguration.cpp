#include "pchAOS_Base.hpp"
#include "AOSConfiguration.hpp"
#include "ALock.hpp"
#include "AOSAdmin.hpp"
#include "AOSController.hpp"
#include "AFile_Physical.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AUrl.hpp"
#include "ALog.hpp"
#include "AFileSystem.hpp"
#include "AOSServices.hpp"
#include "AINIProfile.hpp"

const AString AOSConfiguration::DATABASE_URL("/config/server/database/url");
const AString AOSConfiguration::DATABASE_CONNECTIONS("/config/server/database/connections");

#define DEFAULT_AOSCONTEXTMANAGER_HISTORY_MAX_SIZE 100
#define DEFAULT_AOSCONTEXTMANAGER_FREESTORE_MAX_SIZE 50

void AOSConfiguration::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSConfiguration @ " << std::hex << this << std::dec << ") {" << std::endl;

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
    //a_Controllers
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
    //a_Directory configs
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

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSConfiguration::getClass() const
{
  static const AString CLASS("AOSConfiguration");
  return CLASS;
}

void AOSConfiguration::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);
  
  adminAddProperty(eBase, ASWNL("aos_base_config_directory"), getAosBaseConfigDirectory());
  adminAddProperty(eBase, ASWNL("aos_base_dynamic_directory"), getAosBaseDynamicDirectory());
  adminAddProperty(eBase, ASWNL("aos_base_static_directory"), getAosBaseStaticDirectory());
  adminAddProperty(eBase, ASWNL("aos_base_data_directory"), getAosBaseDataDirectory());

  adminAddProperty(eBase, ASWNL("reported_server")    , m_ReportedServer);
  adminAddProperty(eBase, ASWNL("reported_hostname")  , m_ReportedHostname);
  adminAddProperty(eBase, ASWNL("reported_http_port") , AString::fromInt(m_ReportedHttpPort));
  adminAddProperty(eBase, ASWNL("reported_https_port"), AString::fromInt(m_ReportedHttpsPort));

  {
    LIST_AString moduleNames;
    ARope rope;
    if (getDynamicModuleLibraries(moduleNames) > 0)
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

  adminAddProperty(
    eBase,
    ASWNL("aos_default_filename"),
    getAosDefaultFilename()
  );

  adminAddProperty(
    eBase,
    ASWNL("aos_default_input_processor"),
    getAosDefaultInputProcessor()
  );

  adminAddProperty(
    eBase,
    ASWNL("aos_default_output_generator"),
    getAosDefaultOutputGenerator()
  );

  //a_Controllers
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

  m_AosBaseConfigDir.usePathNames().push_back("conf");
  m_AosBaseDynamicDir.usePathNames().push_back("dynamic");
  m_AosBaseDataDir.usePathNames().push_back("data");
  m_AosBaseStaticDir.usePathNames().push_back("static");
  m_AdminBaseHttpDir.usePathNames().push_back("admin");

  //a_TODO: This needs to be a paremeter
  loadConfig("AObjectServer");

  //a_Get compressed extensions
  _populateGzipCompressionExtensions();

  //a_Configure server internals from config
  setAosDefaultFilename("/config/server/default-filename");

  //a_Configure server reported values
  setReportedServer("/config/server/reported/server");
  setReportedHostname("/config/server/reported/hostname");
  setReportedHttpPort("/config/server/reported/http");
  setReportedHttpsPort("/config/server/reported/https");

  int logLevel = m_Config.useRoot().getInt("/config/server/log-level", -1);
  if (-1 != logLevel)
  {
    m_Services.useLog().setEventMask(ALog::SILENT);
    switch(logLevel)
    {
      case 5 : m_Services.useLog().addEventMask(ALog::ALL);
        break;

      case 4 : m_Services.useLog().addEventMask(ALog::ALL_INFO);
      case 3 : m_Services.useLog().addEventMask(ALog::ALL_MESSAGES);
      case 2 : m_Services.useLog().addEventMask(ALog::ALL_WARNINGS);
      case 1 : m_Services.useLog().addEventMask(ALog::ALL_ERRORS);
        break;

      default: m_Services.useLog().setEventMask(ALog::DEFAULT);
    }
  }

  try
  {
    _readMIMETypes();
    _loadControllers();
  }
  catch(AException& ex)
  {
    //a_Log any exception and re-throw since we just created the log
    services.useLog().addException(ex);
    throw;
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
          //a_Creates too much spam on startup m_Services.useLog().add(ASWNL("Adding MIME type"), strName, strValue, ALog::INFO);
        }
        else
        {
          m_Services.useLog().add(ASWNL("Ignoring duplicate MIME type"), strName, strValue, filename, ALog::WARNING);
        }
      }
    }
  }
}

const AFilename& AOSConfiguration::getBaseDir() const
{
  return m_BaseDir;
}

bool AOSConfiguration::getMimeTypeFromExt(AString strExt, AString& target) const
{
  //a_No extension, no mapping
  if (strExt.isEmpty())
    return false;

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

void AOSConfiguration::loadConfig(const AString& name)
{
  AFilename filename(m_AosBaseConfigDir);
  filename.useFilename().assign(name);
  filename.setExtension("xml");
  
  if (AFileSystem::exists(filename))
  {
    m_Services.useLog().add(ASW("Reading XML config: ",20), filename, ALog::DEBUG);
    
    AFile_Physical file(filename);
    file.open();
    AXmlDocument configDoc(file);
    m_Config.useRoot().addContent(configDoc.useRoot());
  }
  else
    m_Services.useLog().add(ASW("XML config does not exist: ",27), filename, ALog::WARNING);
}

void AOSConfiguration::_loadControllers()
{
  AFileSystem::LIST_FileInfo fileList, directoryConfigs;
  const AString& EXT1 = ASW("aos",3);
  const AString& EXT2 = ASW("xml",3);
  if (AFileSystem::dir(m_AosBaseDynamicDir, fileList, true, true) > 0)
  {
    for(AFileSystem::LIST_FileInfo::iterator it = fileList.begin(); it != fileList.end(); ++it)
    {
      if (!it->filename.equalsExtension(EXT1, EXT2))
      {
        continue;
      }
      
      if (it->filename.useFilename().equals(ASW("__this__.aos.xml",16)))
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
    m_Services.useLog().add(str, m_AosBaseDynamicDir, ALog::WARNING);
  }
}

void AOSConfiguration::_readDirectoryConfig(AFilename& filename)
{
  //a_Open config file before mangling the path
  AFile_Physical configFile(filename);
  AString strPath(1536, 512);

  //a_Add directory config to map
  filename.removeBasePath(m_AosBaseDynamicDir, true);   //a_Remove dynamic dir
  filename.emitPath(strPath);                           //a_Path only for map key
  AASSERT_EX(this, m_DirectoryConfigs.find(strPath) == m_DirectoryConfigs.end(), filename);
  
  {
    AString str("Reading directory config file: ");
    str.append(strPath);
    AOS_DEBUGTRACE(str.c_str(), NULL);
  }  

  //a_Parse XML document
  configFile.open();
  AAutoPtr<AXmlDocument> pDoc(new AXmlDocument(configFile));
  configFile.close();
  AASSERT_EX(pDoc, pDoc->useRoot().getName().equals(ASW("config",6)), ARope("Expected <config> as a root element: ")+filename);
  
  //a_Add directory config
  AOSDirectoryConfig *p =  new AOSDirectoryConfig(strPath, pDoc->useRoot(), m_Services.useLog());
  m_DirectoryConfigs[strPath] = p;

  //a_Register the command with admin
  p->adminRegisterObject(m_Services.useAdminRegistry());
}

void AOSConfiguration::_readController(AFilename& filename)
{
  static const AString CONTROLLER_ROOT(AString("/",1)+AOSController::S_CONTROLLER);
  
  //a_Load from all XML command files
  AFile_Physical commandFile(filename);
  commandFile.open();
  AXmlDocument doc(commandFile);
  commandFile.close();

  AXmlElement::CONST_CONTAINER nodes;
  doc.useRoot().find(CONTROLLER_ROOT, nodes);
  
  //a_Parse each /command/command type
  AString strPath(1536, 512);
  AString dynamicDir(m_AosBaseDynamicDir.toAString());
  AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin();
  while(cit != nodes.end())
  {
    const AXmlElement *pElement = dynamic_cast<const AXmlElement *>(*cit);
    if (pElement)
    {
      //a_Get relative path
      filename.emit(strPath);
      AASSERT(this, strPath.getSize() >= dynamicDir.getSize());
      strPath.remove(dynamicDir.getSize()-1);
      strPath.rremove(8);   //a_.aos.xml to be removed

      {
        AString str(strPath);
        str.append('=');
        filename.emit(str);
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }  

      //a_Parse command and associate to relative path
      AAutoPtr<AOSController> p(new AOSController(strPath, m_Services.useLog()));
      p->fromXml(*pElement);
      AASSERT(this, m_ControllerPtrs.end() == m_ControllerPtrs.find(strPath));
      m_ControllerPtrs[strPath] = p;
      p.setOwnership(false);

      //a_Register the command with admin
      p->adminRegisterObject(m_Services.useAdminRegistry());

      strPath.clear();
    }
    ++cit;
  }
}

void AOSConfiguration::_postProcessControllerAndConfig(AFileSystem::LIST_FileInfo& directoryConfigs)
{
  AString strPath(1536, 512);
  for (AFileSystem::LIST_FileInfo::iterator it = directoryConfigs.begin(); it != directoryConfigs.end(); ++it)
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
      if (AConstant::npos == dir.rremoveUntil('/') || AConstant::npos == dir.rremoveUntil('/', false))
        break;
    }
  }
  return NULL;
}

const AFilename& AOSConfiguration::getAosBaseConfigDirectory() const
{
  return m_AosBaseConfigDir;
}

const AFilename& AOSConfiguration::getAosBaseStaticDirectory() const
{
  return m_AosBaseStaticDir;
}

const AFilename& AOSConfiguration::getAosBaseDynamicDirectory() const
{
  return m_AosBaseDynamicDir;
}

const AFilename& AOSConfiguration::getAosBaseDataDirectory() const
{
  return m_AosBaseDataDir;
}

const AString& AOSConfiguration::getAosDefaultFilename() const
{
  return m_AosDefaultFilename;
}

const AFilename& AOSConfiguration::getAdminBaseHttpDir() const
{
  return m_AdminBaseHttpDir;
}

u4 AOSConfiguration::getDynamicModuleLibraries(LIST_AString& target) const
{
  u4 ret = 0;
  AXmlElement::CONST_CONTAINER nodes;
  m_Config.getRoot().find("/config/server/load/module", nodes);
  for(AXmlElement::CONST_CONTAINER::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    AString str;
    (*it)->emitContent(str);
    target.push_back(str);
    ++ret;
  }
  return ret;
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
      m_Services.useLog().add(ARope("Setting reported server: ")+m_ReportedServer, ALog::DEBUG);
    }
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedServer: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHostname(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    AString str;
    m_Config.useRoot().emitString(configPath, str);

    //a_If hostname exists, set, else use default ip
    if (!str.isEmpty())
    {
      m_ReportedHostname.assign(str);
      m_Services.useLog().add(ARope("Setting reported hostname: ")+m_ReportedHostname, ALog::DEBUG);
    }
    else
    {
      m_ReportedHostname.assign(ASocketLibrary::getDefaultIp());
    }
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHostname: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHttpPort(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_ReportedHttpPort = m_Config.useRoot().getInt(configPath, 80);
    m_Services.useLog().add(ARope("Setting reported http port: ")+AString::fromInt(m_ReportedHttpPort), ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHttpPort: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHttpsPort(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_ReportedHttpsPort = m_Config.useRoot().getInt(configPath, 443);
    m_Services.useLog().add(ARope("Setting reported https port: ")+AString::fromInt(m_ReportedHttpsPort), ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHttpsPort: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultFilename(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultFilename);
    m_Services.useLog().add(ARope("Setting default filename: ")+m_AosDefaultFilename, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultFilename: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultInputProcessor(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultInputProcessor);
    m_Services.useLog().add(ARope("Setting default input processor: ")+m_AosDefaultInputProcessor, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultInputProcessor: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultOutputGenerator(const AString& configPath)
{
  if (m_Config.useRoot().exists(configPath))
  {
    m_Config.useRoot().emitString(configPath, m_AosDefaultOutputGenerator);
    m_Services.useLog().add(ARope("Setting default output generator: ")+m_AosDefaultOutputGenerator, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultOutputGenerator: Unable to find path: ")+configPath, ALog::WARNING);
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
  return m_Config.getRoot().getBool(ASW("/config/server/debug/allow-dumpContext",38), false);
}

bool AOSConfiguration::isOutputOverrideAllowed() const
{
  return m_Config.getRoot().getBool(ASW("/config/server/debug/allow-overrideOutput",41), false);
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

void AOSConfiguration::setConfigBits(
  ConfigBit bit, 
  bool state         // = true
) 
{ 
  m_ConfigBits.setBit(bit, state);
}

const ABitArray& AOSConfiguration::getConfigBits() const 
{ 
  return m_ConfigBits; 
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
