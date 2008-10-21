#!/usr/bin/python
import os,sys;
from acommon import *;

context = Context(os.getcwd());
if (context.processArgv(sys.argv) or context.hasNoPlatform()):
  sys.exit(-1);

# Check if this is not really a sync to another path (target and source are same))
if (context.TARGET_DEBUG_32 == context.DEBUG_32):
  if (context.verbose == 1):
    print("TARGET_PATH is not specified, skipping sync step");
  sys.exit(0);

# If output directory is not there, should create it
context.executeScript("make_output_directories.py");
context.executeScript("gather_binaries_ALibrary.py");
  
# ALibrary include
print("|========================== AOS_Base sync binaries ==============================|");
context.syncLibrary("AOS_Base");
print("|========================== AOS_BaseModules sync binaries =======================|");
context.syncLibrary("AOS_BaseModules");
print("|========================== AObjectServer sync binaries =========================|");
context.syncExecutable("AObjectServer");
print("|========================== AOSWatchDog sync binaries ===========================|");
context.syncExecutable("AOSWatchDog");
