#include "pchAOS_Base.hpp"
#include "AOSConfiguration.hpp"
#include "ALock.hpp"
#include "AOSAdmin.hpp"
#include "AOSCommand.hpp"
#include "AFile_Physical.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AUrl.hpp"
#include "ALog.hpp"
#include "AFileSystem.hpp"
#include "AOSServices.hpp"
#include "AINIProfile.hpp"

const AString AOSConfiguration::LISTEN_ADMIN_PORT("/config/server/listen/admin");
const AString AOSConfiguration::LISTEN_HTTP_PORT("/config/server/listen/http");
const AString AOSConfiguration::LISTEN_HTTPS_PORT("/config/server/listen/https");

const AString AOSConfiguration::DATABASE_URL("/config/server/database/url");
const AString AOSConfiguration::DATABASE_CONNECTIONS("/config/server/database/connections");

#define DEFAULT_AOSCONTEXTMANAGER_HISTORY_MAX_SIZE 100
#define DEFAULT_AOSCONTEXTMANAGER_FREESTORE_MAX_SIZE 50

#ifdef __DEBUG_DUMP__
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

  ADebugDumpable::indent(os, indent+1) << "m_Config=" << std::endl;
  m_Config.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_ConfigBits=" << std::endl;
  m_ConfigBits.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_ReportedServer="  << m_ReportedServer << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHostname="  << m_ReportedHostname << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHttpPort="  << m_ReportedHttpPort << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ReportedHttpsPort=" << m_ReportedHttpsPort << std::endl;

  //a_Commands
  ADebugDumpable::indent(os, indent+1) << "m_CommandPtrs={" << std::endl;
  MAP_ASTRING_COMMANDPTR::const_iterator cit = m_CommandPtrs.begin();
  while (cit != m_CommandPtrs.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "={" << std::endl;
    (*cit).second->debugDump(os, indent+3);
    ++cit;
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSConfiguration::getClass() const
{
  static const AString CLASS("AOSConfiguration");
  return CLASS;
}

void AOSConfiguration::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);
  
  addProperty(eBase, ASWNL("aos_base_config_directory"), getAosBaseConfigDirectory());
  addProperty(eBase, ASWNL("aos_base_dynamic_directory"), getAosBaseDynamicDirectory());
  addProperty(eBase, ASWNL("aos_base_static_directory"), getAosBaseStaticDirectory());
  addProperty(eBase, ASWNL("aos_base_data_directory"), getAosBaseDataDirectory());

  addProperty(eBase, ASWNL("reported_server")    , m_ReportedServer);
  addProperty(eBase, ASWNL("reported_hostname")  , m_ReportedHostname);
  addProperty(eBase, ASWNL("reported_http_port") , AString::fromInt(m_ReportedHttpPort));
  addProperty(eBase, ASWNL("reported_https_port"), AString::fromInt(m_ReportedHttpsPort));


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
      addProperty(
        eBase,
        ASWNL("dynamic_module_libraries"),
        rope
      );
    }
  }

  addProperty(
    eBase,
    ASWNL("aos_default_filename"),
    getAosDefaultFilename()
  );

  addProperty(
    eBase,
    ASWNL("aos_default_input_processor"),
    getAosDefaultInputProcessor()
  );

  addProperty(
    eBase,
    ASWNL("aos_default_output_generator"),
    getAosDefaultOutputGenerator()
  );

  //a_Commands
  {
    ARope rope;
    MAP_ASTRING_COMMANDPTR::const_iterator cit = m_CommandPtrs.begin();
    while (cit != m_CommandPtrs.end())
    {
      rope.append((*cit).second->getCommandName());
      rope.append(AString::sstr_CRLF);
      ++cit;
    }
    addProperty(eBase, ASWNL("Commands"), rope);
  }

  {
    ARope rope;
    m_Config.emit(rope, 0);
    AXmlElement& eObject = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("XMLConfig",9));
    addProperty(
      eBase,
      ASW("config",6),
      rope,
      AXmlData::CDataSafe
    );
  }
}

void AOSConfiguration::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("display_name",12), str))
  {
    MAP_ASTRING_COMMANDPTR::iterator it = m_CommandPtrs.find(str);
    if (it != m_CommandPtrs.end())
      (*it).second->processAdminAction(eBase, request);
    else
      addError(eBase, "command not found");
  }
  else
  {
    addError(eBase, "'display_name' input not found");
    return;
  }
}

AOSConfiguration::AOSConfiguration(
  const AFilename& baseDir,
  AOSServices& services,
  ASynchronization& screenSynch
) :
  m_BaseDir(baseDir),
  m_AosBaseConfigDir(baseDir),
  m_AosBaseDataDir(baseDir),
  m_AosBaseStaticDir(baseDir),
  m_AosBaseDynamicDir(baseDir),
  m_AdminBaseHttpDir(baseDir),
  m_Services(services),
  m_ScreenSynch(screenSynch),
  m_Config(ASW("config",6)),
  m_ReportedServer(AOS_SERVER_NAME),
  m_ReportedHostname("localhost"),
  m_ReportedHttpPort(80),
  m_ReportedHttpsPort(443)
{
  registerAdminObject(services.useAdminRegistry());

  m_AosBaseConfigDir.usePathNames().push_back("conf");
  m_AosBaseDynamicDir.usePathNames().push_back("dynamic");
  m_AosBaseDataDir.usePathNames().push_back("data");
  m_AosBaseStaticDir.usePathNames().push_back("static");
  m_AdminBaseHttpDir.usePathNames().push_back("admin");

  //a_TODO: This needs to be a paremeter
  loadConfig("AObjectServer");

  //a_Configure server internals from config
  setAosDefaultFilename("/config/server/default-filename");

  //a_Configure server reported values
  setReportedServer("/config/server/reported/server");
  setReportedHostname("/config/server/reported/hostname");
  setReportedHttpPort("/config/server/reported/http");
  setReportedHttpsPort("/config/server/reported/https");

  int logLevel = getInt("/config/server/log-level", -1);
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

  _readMIMETypes();
  _loadCommands();
}

AOSConfiguration::~AOSConfiguration()
{
  try
  {
    MAP_ASTRING_COMMANDPTR::iterator it = m_CommandPtrs.begin();
    while (it != m_CommandPtrs.end())
    {
      delete (*it).second;
      ++it;
    }
  }
  catch(...) {}
}

void AOSConfiguration::_readMIMETypes()
{
  if (exists(ASWNL("/config/server/mime-types")))
  {
    AString str;
    emitString(ASWNL("/config/server/mime-types"), str);

    AFilename filename(m_BaseDir);
    AFilename fMime(str);
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

void AOSConfiguration::_loadCommands()
{
  LIST_AFilename fileList;
  const AString& EXT1 = ASW("aos",3);
  const AString& EXT2 = ASW("xml",3);
  const u4 EXT_SIZE = 8;  // ".aos.xml" size to remove
  if (AFileSystem::dir(m_AosBaseDynamicDir, fileList, true, true) > 0)
  {
    LIST_AFilename::iterator it = fileList.begin();
    while(it != fileList.end())
    {
      if (!(*it).equalsExtension(EXT1, EXT2))
      {
        ++it;
        continue;
      }
      
      AXmlDocument doc(ASW("root",4));
      { 
        AFile_Physical configfile(*it);
        configfile.open();
              
        //a_Load from all XML command files
        doc.fromAFile(configfile);
      }
      
      AXmlElement::NodeContainer nodes;
      doc.useRoot().find(ASW("/command",8), nodes);
      
      //a_Parse each /command/command type
      AXmlElement::NodeContainer::const_iterator cit = nodes.begin();
      AString strPath(1024, 512);
      AString dynamicDir(m_AosBaseDynamicDir.toAString());
      while(cit != nodes.end())
      {
        AXmlElement *pElement = dynamic_cast<AXmlElement *>(*cit);
        if (pElement)
        {
          //a_Get relative path
          (*it).emit(strPath);
          AASSERT(this, strPath.getSize() >= dynamicDir.getSize());
          strPath.remove(dynamicDir.getSize()-1);
          strPath.rremove(EXT_SIZE);

          //a_Parse command and associate to relative path
          AOSCommand *p = new AOSCommand(strPath, m_Services.useLog());
          p->fromAXmlElement(*pElement);
          m_CommandPtrs[strPath] = p;

          //a_Register the command with admin
          p->registerAdminObject(m_Services.useAdminRegistry());

          {
            ALock lock(m_ScreenSynch);
            AString str;
            (*it).emit(str);
            std::cout << strPath << "  =  " << str << std::endl;
          }  
          strPath.clear();

        }
        ++cit;
      }
      ++it;
    }
  }
  else
  {
    ALock lock(m_ScreenSynch);
    std::cerr << "WARNING: No command files found at: '" << m_AosBaseDynamicDir << "'" << std::endl;
    m_Services.useLog().add(ASWNL("No command files found"), m_AosBaseDynamicDir, ALog::WARNING);
  }
}
const AOSCommand * const AOSConfiguration::getCommand(const AUrl& commandUrl) const
{
  AString command(commandUrl.getPathAndFilename());
  if (commandUrl.getFilename().isEmpty())
    command.append(m_AosDefaultFilename);

  MAP_ASTRING_COMMANDPTR::const_iterator cit = m_CommandPtrs.find(command);
  if (cit != m_CommandPtrs.end())
    return (*cit).second;
  else
    return NULL;
}

void AOSConfiguration::dumpCommands(AOutputBuffer& target) const
{
  MAP_ASTRING_COMMANDPTR::const_iterator cit = m_CommandPtrs.begin();
  while (cit != m_CommandPtrs.end())
  {
    target.append(" NAME    : ", 12);
    target.append((*cit).second->getCommandName());

    target.append("\r\n INPUT  : ", 12);
    target.append((*cit).second->getInputProcessorName());
    (*cit).second->getInputParams().emit(target);

    target.append("\r\n OUTPUT : ", 12);
    target.append((*cit).second->getOutputGeneratorName());
    (*cit).second->getOutputParams().emit(target);

    target.append("\r\n MODULES: ", 12);

    const AOSCommand::MODULES& modules = (*cit).second->getModuleInfoContainer();
    AOSCommand::MODULES::const_iterator citModule = modules.begin();
    while (citModule != modules.end())
    {
      target.append((*citModule).m_Name);
      target.append(" (",2);
      (*citModule).m_ModuleParams.emit(target);
      target.append(" )",2);
      target.append(AString::sstr_EOL);

      ++citModule;
    }
    ++cit;
    target.append("\r\n\r\n", 4);
  }
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
  AXmlNode::ConstNodeContainer nodes;
  m_Config.getRoot().find("/config/server/load/module", nodes);
  for(AXmlNode::ConstNodeContainer::iterator it = nodes.begin(); it != nodes.end(); ++it)
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
  if (exists(configPath))
  {
    m_ReportedServer.clear();
    emitString(configPath, m_ReportedServer);
    m_Services.useLog().add(ARope("Setting reported server: ")+m_ReportedServer, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedServer: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHostname(const AString& configPath)
{
  if (exists(configPath))
  {
    m_ReportedHostname.clear();
    emitString(configPath, m_ReportedHostname);
    m_Services.useLog().add(ARope("Setting reported hostname: ")+m_ReportedHostname, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHostname: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHttpPort(const AString& configPath)
{
  if (exists(configPath))
  {
    m_ReportedHttpPort = getInt(configPath, 80);
    m_Services.useLog().add(ARope("Setting reported http port: ")+AString::fromInt(m_ReportedHttpPort), ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHttpPort: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setReportedHttpsPort(const AString& configPath)
{
  if (exists(configPath))
  {
    m_ReportedHttpsPort = getInt(configPath, 443);
    m_Services.useLog().add(ARope("Setting reported https port: ")+AString::fromInt(m_ReportedHttpsPort), ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setReportedHttpsPort: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultFilename(const AString& configPath)
{
  if (exists(configPath))
  {
    emitString(configPath, m_AosDefaultFilename);
    m_Services.useLog().add(ARope("Setting default filename: ")+m_AosDefaultFilename, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultFilename: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultInputProcessor(const AString& configPath)
{
  if (exists(configPath))
  {
    emitString(configPath, m_AosDefaultInputProcessor);
    m_Services.useLog().add(ARope("Setting default input processor: ")+m_AosDefaultInputProcessor, ALog::DEBUG);
  }
  else
    m_Services.useLog().add(ARope("AOSConfiguration::setAosDefaultInputProcessor: Unable to find path: ")+configPath, ALog::WARNING);
}

void AOSConfiguration::setAosDefaultOutputGenerator(const AString& configPath)
{
  if (exists(configPath))
  {
    emitString(configPath, m_AosDefaultOutputGenerator);
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

//size_t AOSConfiguration::getAOSContextManagerHistoryMaxSize() const
//{
//  AString str;
//  if (m_Ini.getValue(ASW("AOSContextManager",17), ASW("history_max_size",16), str))
//    return str.toSize_t();
//  else
//    return DEFAULT_AOSCONTEXTMANAGER_HISTORY_MAX_SIZE;
//}
//
//size_t AOSConfiguration::getAOSContextManagerFreestoreMaxSize() const
//{
//  AString str;
//  if (m_Ini.getValue(ASW("AOSContextManager",17), ASW("freestore_max_size",18), str))
//    return str.toSize_t();
//  else
//    return DEFAULT_AOSCONTEXTMANAGER_FREESTORE_MAX_SIZE;
//}

//int AOSConfiguration::isHttpPipeliningEnabled() const
//{
//  static int value = -1;
//  if (-1 == value)
//  {
//    AString str;
//    if (m_Ini.getValue(CONFIG, ASW("http11_pipelining_enabled",25), str))
//      value = str.toInt();
//    else
//      value = 0;
//  }
//  return (0 != value);
//}

const AXmlElement& AOSConfiguration::getConfigRoot() const
{
  return m_Config.getRoot();
}

bool AOSConfiguration::exists(const AString& path) const
{
  return (NULL != m_Config.getRoot().findNode(path));
}

bool AOSConfiguration::emitString(const AString& path, AOutputBuffer& target) const
{
  return m_Config.getRoot().emitFromPath(path, target);
}

AString AOSConfiguration::getString(const AString& path, const AString& strDefault) const
{
  AString str;
  if (m_Config.getRoot().emitFromPath(path, str))
    return str;
  else
    return strDefault;
}          

int AOSConfiguration::getInt(const AString& path, int iDefault) const
{
  AString str;
  if (m_Config.getRoot().emitFromPath(path, str))
    return str.toInt();
  else
    return iDefault;
}

size_t AOSConfiguration::getSize_t(const AString& path, size_t iDefault) const
{
  AString str;
  if (m_Config.getRoot().emitFromPath(path, str))
    return str.toSize_t();
  else
    return iDefault;
}

bool AOSConfiguration::getBool(const AString& path, bool boolDefault) const
{
  AString str;
  if (m_Config.getRoot().emitFromPath(path, str))
  {
    return str.equalsNoCase("true") || str.equals("1");
  }
  else
    return boolDefault;
}

void AOSConfiguration::setString(const AString& path, const AString& value, AXmlData::Encoding encoding)
{
  m_Config.useRoot().addElement(path, value, encoding);
}

void AOSConfiguration::setInt(const AString& path, int value)
{
  m_Config.useRoot().addElement(path, AString::fromInt(value));
}

void AOSConfiguration::setSize_t(const AString& path, size_t value)
{
  m_Config.useRoot().addElement(path, value);
}

void AOSConfiguration::setBool(const AString& path, bool value)
{
  //a_TODO: Need a better way to set existing elements
  AXmlNode *pNode = m_Config.useRoot().findNode(path);
  if (pNode)
  {
    pNode->clear();
    pNode->addContentNode(new AXmlData(AString::fromBool(value)));
  }
  else
  {
    m_Config.useRoot().addElement(path, AString::fromBool(value));
  }
}

AXmlElement& AOSConfiguration::useConfigRoot()
{
  return m_Config.useRoot();
}

