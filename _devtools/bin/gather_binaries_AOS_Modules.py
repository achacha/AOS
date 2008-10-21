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
if (context.targetDoesNotExist()):
  print("Target does not exist, gather_binaries_ALibrary.py calling make_output_directories.py, gather_binaries_ALibrary.py, gather_binaries_AObjectServer.py")
  context.executeScript("make_output_directories.py");
  context.executeScript("gather_binaries_ALibrary.py");
  context.executeScript("gather_binaries_AObjectServer.py");
  
# ALibrary include
print("|========================== AOS_Blog sync binaries ===================================|");
context.syncLibrary("AOS_Blog");
print("|========================== AOS_Classified sync binaries =============================|");
context.syncLibrary("AOS_Classified");
print("|========================== AOS_DadaData sync binaries ===============================|");
context.syncLibrary("AOS_DadaData");
print("|========================== AOS_Example sync binaries ================================|");
context.syncLibrary("AOS_Example");
print("|========================== AOS_Forum sync binaries ==================================|");
context.syncLibrary("AOS_Forum");
print("|========================== AOS_Test sync binaries ===================================|");
context.syncLibrary("AOS_Test");
