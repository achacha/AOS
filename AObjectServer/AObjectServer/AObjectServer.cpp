
#include "pchAObjectServer.hpp"

#include "AOS.hpp"
#include "AOSAdmin.hpp"
#include "AOSServices.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ADynamicLibrary.hpp"

#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

ADynamicLibrary dllModules;                  //a_Map of DLLs loaded
ASocketLibrary_SSL g_SecureSocketLibrary;    //a_Global init for SSL and socket library

static const AString _BUILD_INFO_(AString(AOS_SERVER_NAME)+"\tBUILD("+__TIME__+" "+__DATE__+")");

void traceMultiline(const AString& str, void *ptr)
{
  AString strLine;
  AFile_AString strfile(str);
  while(AConstant::npos != strfile.readLine(strLine))
  {
    AOS_DEBUGTRACE(strLine.c_str(), ptr);
    strLine.clear();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// AObjectServer MAIN /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  AFILE_TRACER_DEBUG_SCOPE((AString("AObjectServer::main:",20)+ASWNL(argv[0])).c_str(), NULL);

  std::cout << _BUILD_INFO_ << std::endl;
  try
  {
    //a_Check if INI filename was provided
    AFilename basePath("./aos_root/",11);
    if (argc > 1)
    {
      //a_Check existance of INI file
      basePath.set(argv[1], true);
    }

    if (!AFileSystem::exists(basePath))
    {
      AString str("main: Base path does not exist: ");
      basePath.emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    //a_Initialize services
    AOSServices services(basePath);

    //a_Admin port
    int admin_port = services.useConfiguration().getInt(AOSConfiguration::LISTEN_ADMIN_PORT, -1);
    if (-1 != admin_port && !ASocketLibrary::canBindToPort(admin_port))
    {
      AString str("main: Unable to bind to admin port ");
      str.append(AString::fromInt(admin_port));
      str.append(" already in use.\r\n");
      services.useConfiguration().emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    //a_HTTP port
    int http_port = services.useConfiguration().getInt(AOSConfiguration::LISTEN_HTTP_PORT, -1);
    if (-1 != http_port && !ASocketLibrary::canBindToPort(http_port))
    {
      AString str("main: Unable to bind to http port ");
      str.append(AString::fromInt(http_port));
      str.append(" already in use.\r\n");
      services.useConfiguration().emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    //a_HTTPS port
    int https_port = services.useConfiguration().getInt(AOSConfiguration::LISTEN_HTTPS_PORT, -1);
    if (-1 != https_port && !ASocketLibrary::canBindToPort(https_port))
    {
      AString str("main: Unable to bind to https port ");
      str.append(AString::fromInt(https_port));
      str.append(" already in use.\r\n");
      services.useConfiguration().emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    if (-1 == admin_port && -1 == https_port && -1 == https_port)
    {
      //a_Nothing to listen to
      AString str("main: Found no ports to listen to.\r\n");
      services.useConfiguration().emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    //a_Ready to start
    {
      AString str;
      ATime().emitRFCtime(str);
      services.useLog().add(ASWNL("=+=+=+= AOS Server starting at ")+str, AString(_BUILD_INFO_));
    }

    try
    {
      AOSAdmin admin(services);
      services.registerAdminObject(services.useAdminRegistry());

      AString str;

      //  AOSContextQueue_HttpReader cqHttpReader(services, 6);
      AOSContextQueue_IsAvailable cqIsAvailable(services);

      AOSContextQueue_PreExecutor cqPreExecutor(services, 5);
      AOSContextQueue_Executor cqExecutor(services, 12);
      AOSContextQueue_ErrorExecutor cqErrorExecutor(services);

      //a_Connect queues to listener
      AOSRequestListener listener(services, &cqPreExecutor);
      
      cqIsAvailable.setYesContextQueue(&cqPreExecutor);

      cqPreExecutor.setYesContextQueue(&cqExecutor);
      cqPreExecutor.setNoContextQueue(&cqIsAvailable);
      cqPreExecutor.setErrorContextQueue(&cqErrorExecutor);

      cqExecutor.setYesContextQueue(&cqPreExecutor);                 //a_Used in HTTP/1.1 pipelining
      cqExecutor.setErrorContextQueue(&cqErrorExecutor);

      //a_Start all the queues (listener is the last thing to start, see below)
      cqIsAvailable.useThreadPool().start();
      cqPreExecutor.useThreadPool().start();
      cqExecutor.useThreadPool().start();
      cqErrorExecutor.useThreadPool().start();

      //
      //a_Initialize the global objects
      //
      AString strError;
      if (services.initDatabasePool())
      {
        size_t rows = services.loadGlobalObjects(strError);
        if (AConstant::npos == rows)
        {
          AOS_DEBUGTRACE((AString("DBERROR: Global init: ")+strError).c_str(), NULL);
          return -1;
        }
        else
        {
          str.clear();
          str.assign("Database connection pool connected ");
          services.useConfiguration().emitString(AOSConfiguration::DATABASE_CONNECTIONS, str);
          str.append("x to ");
          services.useConfiguration().emitString(AOSConfiguration::DATABASE_URL, str);
          AOS_DEBUGTRACE(str.c_str(), NULL);
          str.assign("Querying for global variables, processed ");
          str.append(AString::fromSize_t(rows));
          str.append(" rows.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
        }
      }
      else
      {
        AOS_DEBUGTRACE("Database was not initialized, skipping global init.", NULL);
      }

      //
      //a_Load the processors, modules, generators dynamically from DLL
      //
      LIST_AString listModules;
      services.useConfiguration().getDynamicModuleLibraries(listModules);

      //a_Load modules
      LIST_AString::const_iterator cit = listModules.begin();
      while(cit != listModules.end())
      {
        if (!dllModules.load(*cit))
        {
          AString strDebug("Unable to load dynamic library: ");
          strDebug.append(*cit);
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        {
          AString strDebug("=====[ BEGIN: Processing library '");
          strDebug.append(*cit);
          strDebug.append("' ]=====");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        //a_Load the corresponding XML config for each library if exists
        services.useConfiguration().loadConfig(*cit);

        //a_Register modules
        PROC_AOS_Register *procRegister = static_cast<PROC_AOS_Register *>(dllModules.getEntryPoint(*cit, "aos_register"));
        if (procRegister)
        {
          if (
            procRegister(
              cqExecutor.useAOSInputExecutor(), 
              cqExecutor.useAOSModuleExecutor(), 
              cqExecutor.useAOSOutputExecutor(), 
              services.useLog()
            )
          )
          {
            AString strDebug("  FAILED to register module: '");
            strDebug.append(*cit);
            strDebug.append('\'');
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
          else
          {
            AString strDebug("  Registered: '");
            strDebug.append(*cit);
            strDebug.append('\'');
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
        }
        else
        {
          AString strDebug("  Module: '");
          strDebug.append(*cit);
          strDebug.append("': unable to find proc symbol: aos_register");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        //a_Initialize against global object container
        PROC_AOS_Init *procInit = static_cast<PROC_AOS_Init *>(dllModules.getEntryPoint(*cit, "aos_init"));
        if (procInit)
        {
          if (procInit(services))
          {
            AString strDebug("  FAILED to initialize module: '");
            strDebug.append(*cit);
            strDebug.append('\'');
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
          else
          {
            AString strDebug("  Initialized: '");
            strDebug.append(*cit);
            strDebug.append("'");
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
        }
        else
        {
          AString strDebug("  Module: '");
          strDebug.append(*cit);
          strDebug.append("': unable to find proc symbol: aos_init");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        {
          AString strDebug("=====[   END: Processing library '");
          strDebug.append(*cit);
          strDebug.append("' ]=====");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }
        ++cit;
      }

      //a_TODO: Should verify that defaults configured have been loaded...
                  
      //a_Start listener
      listener.startListening();

      //a_Start admin listener
      admin.startAdminListener();

      do
      {
        //a_This is the watcher for the main loop, when admin is not running the server has stopped
        AThread::sleep(3000);
      }
      while (admin.isRunning());
    }
    catch(AException& ex)
    {
      services.useLog().append(ex);
      throw;
    }
    catch(...)
    {
      services.useLog().append("Unknown exception caught, server exiting.");
      throw;
    }
  }
  catch(AException& ex)
  {
    AString str("main: ");
    str.append(ex);
    traceMultiline(str, NULL);
  }
  catch(...)
  {
    AOS_DEBUGTRACE("Unknown exception caught, server exiting.", NULL);
  }
 
  return 0;
}
