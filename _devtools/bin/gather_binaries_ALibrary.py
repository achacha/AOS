#!/usr/bin/python
import os,sys;
from acommon import *;
  
context = Context(os.getcwd());
if (context.processArgv(sys.argv) or context.hasNoPlatform()):
  context.showUsage();
  sys.exit(-1);

# Check if this is not really a sync to another path (target and source are same))
if (context.TARGET_DEBUG_32 == context.DEBUG_32):
  if (context.verbose == 1):
    print("TARGET_PATH is not specified, skipping sync step");
  sys.exit(0);

# If output directory is not there, should create it
if (context.targetDoesNotExist()):
  print("Target does not exist, gather_binaries_ALibrary.py calling make_output_directories.py")
  context.executeScript("make_output_directories.py");

sys.exit(0);
  
# ALibrary include
print("|========================== ABase sync binaries ==============================|");
context.syncLibrary("ABase");
print("|========================== ACrypt sync binaries =============================|");
context.syncLibrary("ACrypto");
print("|========================== ADatabase_MySQL sync binaries ====================|");
context.syncLibrary("ADatabase_MySQL");
print("|========================== ADatabase_ODBC sync binaries =====================|");
context.syncLibrary("ADatabase_ODBC");
print("|========================== ADatabase_SQLite sync binaries ===================|");
context.syncLibrary("ADatabase_SQLite");
print("|========================== AGdLib sync binaries =============================|");
context.syncLibrary("AGdLib");
print("|========================== ALuaEmbed sync binaries ==========================|");
context.syncLibrary("ALuaEmbed");
