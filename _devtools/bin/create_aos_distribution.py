#!/bin/python
import os,sys;
from acommon import *;

context = Context(os.getcwd());
if (context.processArgv(sys.argv) == -1 or context.hasNoPlatform()):
  context.showUsage();
  sys.exit(-1);

# Gather headers
if (context.binary == 0):
  context.executeScript("gather_headers.py");

# Create output directories and binaries
context.executeScript("make_output_directories.py");

# Gather ALibrary binaries
context.executeScript("gather_binaries_ALibrary.py");

# Gather AOS_Modules
print("|========================== AOS_Classified sync binaries =============================|");
context.syncLibrary("AOS_Classified");
print("|========================== AOS_Example sync binaries ================================|");
context.syncLibrary("AOS_Example");
print("|========================== AOS_Test sync binaries ===================================|");
context.syncLibrary("AOS_Test");

# Make AOS configuration: AObjectServer
context.syncAosRoot(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AObjectServer"));
context.syncAosRoot(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AOS_BaseModules"));

# Make AOS configuration: AOS_Modules
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Classified"));
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Example"));
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Test"));
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_User"));
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Wiki"));
context.syncAosRoot(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_DadaData"));

# AObjectServer and AOS_Modules
if (context.binary == 0):
  # Copy AObjectServer and AOS_Modules source
  print "----------AObjectServer base--------";
  if (not os.path.exists(os.path.join(context.TARGET_PATH, "AObjectServer"))):
    os.makedirs(os.path.join(context.TARGET_PATH, "AObjectServer"));
  context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AObjectServer"), "*.*", os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServer"));
  context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH), "AObjectServerWithoutALibrary.sln", os.path.join(context.TARGET_PATH, "AObjectServer"));
  if (os.path.exists(os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServer.sln"))):
    os.remove(os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServer.sln"));
  os.rename(os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServerWithoutALibrary.sln"), os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServer.sln"))


  print "----------AObjectServer--------";
  context.syncPath(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AObjectServer", "*.*"), os.path.join(context.TARGET_PATH, "AObjectServer", "AObjectServer"));

  print "----------AOS_Base--------";
  context.syncPath(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AOS_Base", "*.*"), os.path.join(context.TARGET_PATH, "AObjectServer", "AOS_Base"));

  print "----------AOS_BaseModules--------";
  context.syncPath(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AOS_BaseModules", "*.*"), os.path.join(context.TARGET_PATH, "AObjectServer", "AOS_BaseModules"));

  print "----------AOSWatchDog--------";
  context.syncPath(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AOSWatchDog", "*.*"), os.path.join(context.TARGET_PATH, "AObjectServer", "AOSWatchDog"));

  # AOS_Modules
  print "----------AOS_Modules--------";
  if (not os.path.exists(os.path.join(context.TARGET_PATH, "AOS_Modules"))):
    os.makedirs(os.path.join(context.TARGET_PATH, "AOS_Modules"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Classified"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_Classified"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Example"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_Example"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Test"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_Test"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_User"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_User"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_Wiki"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_Wiki"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH, "AOS_DadaData"), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_DadaData"));
  context.syncPathFilesWithPCH(os.path.join(context.BASE_AOSMODULES_PATH), "*.*", os.path.join(context.TARGET_PATH, "AOS_Modules"));
  os.system("rm "+os.path.join(context.TARGET_PATH, "AOS_Modules", "AOS_Modules.*"));
  os.remove(os.path.join(context.TARGET_PATH, "AOS_Modules", "lint.cmd"));
  
  # Hive executable
  context.syncExecutable("Hive");

else:
  #Copy AObjectServer and AOS_Modules binaries
  print("--------------Sync executables---------------------");
  context.syncExecutable("AObjectServer");
  context.syncExecutable("AOSWatchDog");
  context.syncLibrary("AOS_Base");
  context.syncLibrary("AOS_BaseModules");
  context.syncLibrary("AOS_Classified");
  context.syncLibrary("AOS_Example");
  context.syncLibrary("AOS_Test");
  context.syncLibrary("AOS_User");
  context.syncLibrary("AOS_Wiki");
  context.syncLibrary("AOS_DadaData");
  
# Extras
context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH), "GETTING_STARTED.txt", context.TARGET_PATH);
context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH), "CHANGELOG.txt", context.TARGET_PATH);
context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH), "LICENSE.txt", context.TARGET_PATH);
context.syncPathFiles(os.path.join(context.BASE_AOBJECTSERVER_PATH), "CONTRIBUTORS.txt", context.TARGET_PATH);

