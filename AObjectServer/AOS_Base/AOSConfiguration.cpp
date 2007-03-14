#include "pchAOS_Base.hpp"
#include "AOSConfiguration.hpp"
#include "ALock.hpp"
#include "AOSAdmin.hpp"
#include "AOSCommand.hpp"
#include "AFile_Physical.hpp"
#include "AXmlDocument.hpp"
#include "AUrl.hpp"
#include "ALog.hpp"
#include "AFileSystem.hpp"
#include "AOSServices.hpp"

#define DEFAULT_INI_FILENAME ASW("AObjectServer.ini",17)

const AString AOSConfiguration::CONFIG("config");

#define DEFAULT_AOS_SERVER_PORT 10080
#define DEFAULT_AOS_SECURE_SERVER_PORT 10443

#define DEFAULT_REPORTED_SERVER_PORT 80
#define DEFAULT_REPORTED_SECURE_SERVER_PORT 443
#define DEFAULT_REPORTED_SERVER_NAME "127.0.0.1"

#define DEFAULT_AOS_CERT_FILENAME "aosconfig/aos_cert.pem"
#define DEFAULT_AOS_PKEY_FILENAME "aosconfig/aos_pkey.pem"

#define DEFAULT_ADMIN_SERVER_PORT 12345

#define DEFAULT_FIRST_CHAR_READ_TRIES 12
#define DEFAULT_DATABASE_MAX_CONNECTIONS 6

#define DEFAULT_AOS_DEFAULT_FILENAME "index.html"

#define AOS_CONFIG_BASE_PATH "aos_config_base_path"
#define AOS_STATIC_BASE_PATH "aos_static_base_path"
#define AOS_DYNAMIC_BASE_PATH "aos_dynamic_base_path"
#define AOS_DATA_BASE_PATH "aos_data_base_path"
#define AOS_DEFAULT_FILENAME "aos_default_filename"
#define AOS_DEFAULT_INPUT_PROCESSOR "aos_default_input_processor"
#define AOS_DEFAULT_OUTPUT_GENERATOR "aos_default_output_generator"
#define ADMIN_BASE_PATH "admin_base_path"

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

  ADebugDumpable::indent(os, indent+1) << "m_Ini=" << std::endl;
  m_Ini.debugDump(os, indent+2);

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
  
  addProperty(
    eBase,
    ASWNL("aos_base_config_directory"),
    getAosBaseConfigDirectory()
  );

  addProperty(
    eBase,
    ASWNL("aos_base_dynamic_directory"),
    getAosBaseDynamicDirectory()
  );

  addProperty(
    eBase,
    ASWNL("aos_base_static_directory"),
    getAosBaseStaticDirectory()
  );

  addProperty(
    eBase,
    ASWNL("aos_base_data_directory"),
    getAosBaseDataDirectory()
  );

  {
    AUrl url;
    if (getDatabaseUrl(url))
      addProperty(
        eBase,
        ASWNL("database_url"),
        url
      );
  }

  addProperty(
    eBase,
    ASWNL("database_max_connections"),
    AString::fromInt(getDatabaseMaxConnections())
  );

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

  addProperty(
    eBase,
    ASWNL("http_first_char_read_tries"),
    AString::fromInt(getHttpFirstCharReadTries())
  );

  addProperty(
    eBase,
    ASWNL("aos_server_port"),
    AString::fromInt(getAosServerPort())
  );


  addProperty(
    eBase,
    ASWNL("reported_server_name"),
    getReportedServerName()
  );

  addProperty(
    eBase,
    ASWNL("reported_server_port"),
    AString::fromInt(getReportedServerPort())
  );

  //a_Commands
  MAP_ASTRING_COMMANDPTR::const_iterator cit = m_CommandPtrs.begin();
  while (cit != m_CommandPtrs.end())
  {
    AXmlElement& eObject = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("AOSCommand",10));
    (*cit).second->addAdminXml(eObject, request);
    ++cit;
  }

  //a_Ini
  {
    ARope rope;
    m_Ini.emit(rope);
    AXmlElement& eObject = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("AINIProfile",11));
    addProperty(
      eObject,
      ASWNL("ini"),
      rope
    );
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
  const AFilename& configDir,
  AOSServices& services,
  ASynchronization& screenSynch
) :
  m_BaseConfigDir(configDir),
  m_Services(services),
  m_ScreenSynch(screenSynch),
  m_Config(ASW("config",6))
{
  registerAdminObject(services.useAdminRegistry());

  AFilename iniFilename(configDir);
  iniFilename.useFilename().assign(DEFAULT_INI_FILENAME);
  m_Ini.setFilename(iniFilename);
  m_Ini.parse();

  AString strLogLevel;
  if (m_Ini.getValue(CONFIG, ASW("log_level",9), strLogLevel))
  {
    m_Services.useLog().setEventMask(ALog::SILENT);
    switch(strLogLevel.toInt())
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

  iniFilename.emitPath(m_AosBaseConfigDir);
  if (!m_Ini.getValue(CONFIG, AOS_CONFIG_BASE_PATH, m_AosBaseConfigDir))
    ATHROW_EX(this, AException::InvalidConfiguration, "Missing required INI entry [config]" AOS_DATA_BASE_PATH);

  iniFilename.emitPath(m_AosBaseDynamicDir);
  if (!m_Ini.getValue(CONFIG, AOS_DYNAMIC_BASE_PATH, m_AosBaseDynamicDir))
    ATHROW_EX(this, AException::InvalidConfiguration, "Missing required INI entry [config]" AOS_DYNAMIC_BASE_PATH);
  
  iniFilename.emitPath(m_AosBaseDataDir);
  if (!m_Ini.getValue(CONFIG, AOS_DATA_BASE_PATH, m_AosBaseDataDir))
    ATHROW_EX(this, AException::InvalidConfiguration, "Missing required INI entry [config]" AOS_DATA_BASE_PATH);

  iniFilename.emitPath(m_AosBaseStaticDir);
  if (!m_Ini.getValue(CONFIG, AOS_STATIC_BASE_PATH, m_AosBaseStaticDir))
    ATHROW_EX(this, AException::InvalidConfiguration, "Missing required INI entry [config]" AOS_STATIC_BASE_PATH);

  if (!m_Ini.getValue(CONFIG, AOS_DEFAULT_FILENAME, m_AosDefaultFilename))
    m_AosDefaultFilename = DEFAULT_AOS_DEFAULT_FILENAME;

  m_Ini.getValue(CONFIG, AOS_DEFAULT_INPUT_PROCESSOR, m_AosDefaultInputProcessor);
  m_Ini.getValue(CONFIG, AOS_DEFAULT_OUTPUT_GENERATOR, m_AosDefaultOutputGenerator);

  iniFilename.emitPath(m_AdminBaseHttpDir);
  if (!m_Ini.getValue(CONFIG, ADMIN_BASE_PATH, m_AdminBaseHttpDir))
    ATHROW_EX(this, AException::InvalidConfiguration, "Missing required INI entry [config]" ADMIN_BASE_PATH);
  
  _readMIMETypes();
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
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("mime_types"), str))
  {
    AFilename filename = getBaseConfigDir();
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

const AFilename& AOSConfiguration::getBaseConfigDir() const
{
  return m_BaseConfigDir;
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
  AFilename filename(m_AosBaseConfigDir, name, false);
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
    m_Services.useLog().add(ASW("XML config does not exist: ",27), filename, ALog::DEBUG);
}

void AOSConfiguration::loadCommands(AOSServices& services)
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
      while(cit != nodes.end())
      {
        AXmlElement *pElement = dynamic_cast<AXmlElement *>(*cit);
        if (pElement)
        {
          //a_Get relative path
          (*it).emit(strPath);
          AASSERT(this, strPath.getSize() >= m_AosBaseDynamicDir.getSize());
          strPath.remove(m_AosBaseDynamicDir.getSize()-1);
          strPath.rremove(EXT_SIZE);

          //a_Parse command and associate to relative path
          AOSCommand *p = new AOSCommand(strPath, m_Services.useLog());
          p->fromAXmlElement(*pElement);
          m_CommandPtrs[strPath] = p;

          //a_Register the command with admin
          p->registerAdminObject(services.useAdminRegistry());

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

int AOSConfiguration::getAosServerPort() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("aos_server_port"), str))
    return str.toInt();
  else
    return DEFAULT_AOS_SERVER_PORT;
}

int AOSConfiguration::getAosSecureServerPort() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("aos_secure_server_port"), str))
    return str.toInt();
  else
    return DEFAULT_AOS_SECURE_SERVER_PORT;
}

int AOSConfiguration::getAdminServerPort() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("admin_server_port"), str))
    return str.toInt();
  else
    return DEFAULT_ADMIN_SERVER_PORT;
}

int AOSConfiguration::getHttpFirstCharReadTries() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("http_first_char_read_tries"), str))
    return str.toInt();
  else
    return DEFAULT_FIRST_CHAR_READ_TRIES;
}

bool AOSConfiguration::getDatabaseUrl(AUrl& url) const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("database_url"), str))
  {
    url.parse(str);
    return true;
  }
  else
  {
    return false;
  }
}

int AOSConfiguration::getDatabaseMaxConnections() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("database_max_connections"), str))
    return str.toInt();
  else
    return DEFAULT_DATABASE_MAX_CONNECTIONS;
}

const AString& AOSConfiguration::getAosBaseConfigDirectory() const
{
  return m_AosBaseConfigDir;
}

const AString& AOSConfiguration::getAosBaseStaticDirectory() const
{
  return m_AosBaseStaticDir;
}

const AString& AOSConfiguration::getAosBaseDynamicDirectory() const
{
  return m_AosBaseDynamicDir;
}

const AString& AOSConfiguration::getAosBaseDataDirectory() const
{
  return m_AosBaseDataDir;
}

const AString& AOSConfiguration::getAosDefaultFilename() const
{
  return m_AosDefaultFilename;
}

const AString& AOSConfiguration::getAdminBaseHttpDirectory() const
{
  return m_AdminBaseHttpDir;
}

u4 AOSConfiguration::getDynamicModuleLibraries(LIST_AString& target) const
{
  u4 ret = 0;
  AString str(1024, 1024);
  if (m_Ini.getValue(CONFIG, ASWNL("module_libs"), str))
  {
    str.split(target, ',', AString::sstr_WhiteSpace);
  }
  return ret;
}

int AOSConfiguration::getReportedServerPort() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("reported_server_port"), str))
    return str.toInt();
  else
    return DEFAULT_REPORTED_SERVER_PORT;
}

int AOSConfiguration::getReportedSecureServerPort() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("reported_secure_server_port"), str))
    return str.toInt();
  else
    return DEFAULT_REPORTED_SECURE_SERVER_PORT;
}

AString AOSConfiguration::getReportedServerName() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("reported_server_name"), str))
    return str;
  else
    return DEFAULT_REPORTED_SERVER_NAME;
}

AString AOSConfiguration::getAosCertFilename() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("aos_cert_filename"), str))
    return str;
  else
    return DEFAULT_AOS_CERT_FILENAME;
}

AString AOSConfiguration::getAosPKeyFilename() const
{
  AString str;
  if (m_Ini.getValue(CONFIG, ASWNL("aos_pkey_filename"), str))
    return str;
  else
    return DEFAULT_AOS_PKEY_FILENAME;
}

const AString& AOSConfiguration::getAosDefaultInputProcessor() const
{
  return m_AosDefaultInputProcessor;
}

const AString& AOSConfiguration::getAosDefaultOutputGenerator() const
{
  return m_AosDefaultOutputGenerator;
}

size_t AOSConfiguration::getAOSContextManagerHistoryMaxSize() const
{
  AString str;
  if (m_Ini.getValue(ASW("AOSContextManager",17), ASW("history_max_size",16), str))
    return str.toSize_t();
  else
    return DEFAULT_AOSCONTEXTMANAGER_HISTORY_MAX_SIZE;
}

size_t AOSConfiguration::getAOSContextManagerFreestoreMaxSize() const
{
  AString str;
  if (m_Ini.getValue(ASW("AOSContextManager",17), ASW("freestore_max_size",18), str))
    return str.toSize_t();
  else
    return DEFAULT_AOSCONTEXTMANAGER_FREESTORE_MAX_SIZE;
}

int AOSConfiguration::isHttpPipeliningEnabled() const
{
  static int value = -1;
  if (-1 == value)
  {
    AString str;
    if (m_Ini.getValue(CONFIG, ASW("http11_pipelining_enabled",25), str))
      value = str.toInt();
    else
      value = 0;
  }
  return (0 != value);
}

const AXmlElement& AOSConfiguration::getConfigRoot() const
{
  return m_Config.getRoot();
}

bool AOSConfiguration::exists(const AString& path) const
{
  return (NULL != m_Config.getRoot().findNode(path));
}

bool AOSConfiguration::get(const AString& path, AOutputBuffer& target)
{
  return m_Config.getRoot().emitFromPath(path, target);
}
