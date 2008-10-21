#!/usr/bin/python
import os,sys;
from acommon import *;

def makeDirs(context):
  if (context.binary == 0 and (context.win32 == 1 or context.linux32 == 1) and not os.path.exists(context.TARGET_DEBUG_32)):
    os.makedirs(context.TARGET_DEBUG_32);
    if (not os.path.exists(context.TARGET_DEBUG_32)):
      print "Unable to create '"+context.TARGET_DEBUG_32+"', may need manual intervention.";
      sys.exit(-1);

  if ((context.win32 == 1 or context.linux32 == 1) and not os.path.exists(context.TARGET_RELEASE_32)):
    os.makedirs(context.TARGET_RELEASE_32);
    if (not os.path.exists(context.TARGET_RELEASE_32)):
      print "Unable to create '"+context.TARGET_RELEASE_32+"', may need manual intervention.";
      sys.exit(-1);

  if (context.binary == 0 and (context.win64 == 1 or context.linux64 == 1) and not os.path.exists(context.TARGET_DEBUG_64)):
    os.makedirs(context.TARGET_DEBUG_64);
    if (not os.path.exists(context.TARGET_DEBUG_64)):
      print "Unable to create '"+context.TARGET_DEBUG_64+"', may need manual intervention.";
      sys.exit(-1);

  if ((context.win64 == 1 or context.linux64 == 1) and not os.path.exists(context.TARGET_RELEASE_64)):
    os.makedirs(context.TARGET_RELEASE_64);
    if (not os.path.exists(context.TARGET_RELEASE_64)):
      print "Unable to create '"+context.TARGET_RELEASE_64+"', may need manual intervention.";
      sys.exit(-1);

context = Context(os.getcwd());
if (context.processArgv(sys.argv) or context.hasNoPlatform()):
  context.showUsage();
  sys.exit(-1);

print "|---------Creating directories";
makeDirs(context);

##
## Windows 32-bit libraries
##  
if (context.win32 == 1):
  DIST_WIN32_PATH = os.path.join(context.BASE_CODE_PATH, "_dist_windows", "x86");
  
  if (context.clean == 1):
    if (context.binary == 0):
      context.makeSystemCall("rm -rI "+context.TARGET_DEBUG_32);
    context.makeSystemCall("rm -rI "+context.TARGET_RELEASE_32);

  # Base libraries
  print "|---------EXTERNAL: Windows x86-----|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(DIST_WIN32_PATH, "lib", "debug"), "*.*", context.TARGET_DEBUG_32);
  context.syncPathFiles(os.path.join(DIST_WIN32_PATH, "lib", "release"), "*.*", context.TARGET_RELEASE_32);

  # ACrypto
  print "|---------EXTERNAL: ALibraryImpl/ACrypto-----------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "lib", "debug"), "*.*", context.TARGET_DEBUG_32);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "lib", "release"), "*.*", context.TARGET_RELEASE_32);

  # ADatabase_MySQL
  print "|---------EXTERNAL: ALibraryImpl/ADatabase_MySQL-----------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "lib", "debug"), "*.*", context.TARGET_DEBUG_32);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "lib", "release"), "*.*", context.TARGET_RELEASE_32);

  # AZlib
  print "|---------EXTERNAL: ALibraryImpl/AZlib---------------------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib", "lib", "debug"), "*.*", context.TARGET_DEBUG_32);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib", "lib", "release"), "*.*", context.TARGET_RELEASE_32);

  # AGdLib
  print "|---------EXTERNAL: ALibraryImpl/AGdLib--------------------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "lib", "debug"), "*.*", context.TARGET_DEBUG_32);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "lib", "release"), "*.*", context.TARGET_RELEASE_32);


##
## Windows 64-bit libraries
##  
if (context.win64 == 1):
  DIST_WIN64_PATH = os.path.join(context.BASE_CODE_PATH, "_dist_windows", "a64");
  if (context.clean == 1):
    if (context.binary == 0):
      context.makeSystemCall("rm -rI "+context.TARGET_DEBUG_64);
    context.makeSystemCall("rm -rI "+context.TARGET_RELEASE_64);

  # Base libraries
  print "|---------EXTERNAL: Windows 64a-----|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(DIST_WIN64_PATH, "lib", "debug64a"), "*.*", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(DIST_WIN64_PATH, "lib", "release64a"), "*.*", context.TARGET_RELEASE_64);

  # ACrypto
  print "|---------EXTERNAL: ALibraryImpl/ACrypto-----------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "lib", "debug64a"), "*.*", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "lib", "release64a"), "*.*", context.TARGET_RELEASE_64);

  # ADatabase_MySQL
  print "|---------EXTERNAL: ALibraryImpl/ADatabase_MySQL-----------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "lib", "debug64a"), "*.*", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "lib", "release64a"), "*.*", context.TARGET_RELEASE_64);

  # AZlib
  print "|---------EXTERNAL: ALibraryImpl/AZlib---------------------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib", "lib", "debug64a"), "*.*", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib", "lib", "release64a"), "*.*", context.TARGET_RELEASE_64);

  # AGdLib
  print "|---------EXTERNAL: ALibraryImpl/AGdLib--------------------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "lib", "debug64a"), "*.*", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "lib", "release64a"), "*.*", context.TARGET_RELEASE_64);

##
## Common to both windows
##
if (context.win32 == 1 or context.win64 == 1):
  #SQLite3 tools
  print "|---SQLite3 redistributable--------------------------------|";
  if (context.binary == 0):
    context.syncPathFiles(os.path.join(context.BASE_DEVENV_PATH, "sqlite3"), "sqlite3.exe", context.TARGET_DEBUG_32);
    context.syncPathFiles(os.path.join(context.BASE_DEVENV_PATH, "sqlite3"), "sqlite3.exe", context.TARGET_DEBUG_64);
  context.syncPathFiles(os.path.join(context.BASE_DEVENV_PATH, "sqlite3"), "sqlite3.exe", context.TARGET_RELEASE_32);
  context.syncPathFiles(os.path.join(context.BASE_DEVENV_PATH, "sqlite3"), "sqlite3.exe", context.TARGET_RELEASE_64);
