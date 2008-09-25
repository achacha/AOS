#!/bin/python
import os,sys;

THIS_PATH=os.path.dirname(sys.argv[0]);
BASE_PATH = os.path.normpath(os.path.join(THIS_PATH,"..",".."));
ALIBRARY_PATH=os.path.join(BASE_PATH,"ALibrary");
ALIBRARY_PATH=os.path.join(BASE_PATH,"ALibrary");
EXEC_RSYNC_BASE=os.path.join(BASE_PATH,"_devtools","bin","rsync.py")+" -tuC --exclude=CVS ";
EXEC_RSYNC_BASE_WITH_DELETE=os.path.join(BASE_PATH,"_devtools","bin","rsync.py")+" -tuC --exclude=CVS --delete ";

if (not os.path.exists(ALIBRARY_PATH)):
  print "Unable to find ALibrary at "+ALIBRARY_PATH;
  sys.exit(-1);

def makeSystemCall(param):
  if (verbose == 1):
    print param;
  os.system(param);
   
def makeSystemCall_mkdir(dirname):
  if (not os.path.exists(dirname)):
    CMD = "mkdir \""+dirname+"\"";
    if (verbose == 1):
      print CMD;
    os.system(CMD);

def syncPath(source_path, target_path):
  CMD = EXEC_RSYNC_BASE + source_path + " " + target_path; 
  if (verbose == 1):
    print CMD;
  os.system(CMD);

def syncPathWithDelete(source_path, target_path):
  CMD = EXEC_RSYNC_BASE_WITH_DELETE + source_path + " " + target_path; 
  if (verbose == 1):
    print CMD;
  os.system(CMD);

def touch(filename):
  if (not os.path.exists(filename)):
    f = open(filename, "w");
    print >>f,filename;
    f.close();
  

def showUsage():
  print "Usage: "+sys.argv[0]+" [options]";
  print " -p <target path>   - target path = where everything is copied to";
  print " -v                 - verbose mode";
  print " -clean             - clean first";
  print " -help              - this help";

######################################################################
########################## MAIN ######################################
######################################################################

TARGET_PATH = os.path.normpath(os.path.join(ALIBRARY_PATH, ".."));
DEBUG_INPUT_PATH = os.path.join(TARGET_PATH,"_debug64a");
RELEASE_INPUT_PATH = os.path.join(TARGET_PATH,"_release64a");
target_bin_path_DEBUG = DEBUG_INPUT_PATH;
target_bin_path_RELEASE = RELEASE_INPUT_PATH;

verbose = 0;
clean = 0;
argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-clean"):
    clean = 1;
  elif (sys.argv[argc] == "-v"):
    verbose = 1;
  elif (sys.argv[argc] == "-p"):
    TARGET_PATH = sys.argv[argc+1];
    target_bin_path_DEBUG = os.path.normpath(os.path.join(TARGET_PATH,"_debug64a"));
    target_bin_path_RELEASE = os.path.normpath(os.path.join(TARGET_PATH,"_release64a"));
    argc += 1;
  elif (sys.argv[argc] == "-help"):
    showUsage();
    sys.exit(-1);
  else:
    print "Unknown parameter: "+sys.argv[argc];
    showUsage();
    sys.exit(-1);
  
  argc += 1;

if (not os.path.exists(DEBUG_INPUT_PATH)):
  os.makedirs(DEBUG_INPUT_PATH);
  if (not os.path.exists(DEBUG_INPUT_PATH)):
    print "Unable to create '"+DEBUG_INPUT_PATH+"', may need manual intervention.";
    sys.exit(-1);

if (not os.path.exists(RELEASE_INPUT_PATH)):
  os.makedirs(RELEASE_INPUT_PATH);
  if (not os.path.exists(RELEASE_INPUT_PATH)):
    print "Unable to create '"+RELEASE_INPUT_PATH+"', may need manual intervention.";
    sys.exit(-1);

# Target paths
makeSystemCall_mkdir(TARGET_PATH);

# Path for binaries
makeSystemCall_mkdir(target_bin_path_DEBUG);
makeSystemCall_mkdir(target_bin_path_RELEASE);

if (verbose == 1):
  print "Input base path     : "+BASE_PATH;
  print "Input DEBUG path    : "+DEBUG_INPUT_PATH;
  print "Input RELEASE path  : "+RELEASE_INPUT_PATH;
  print "Output target path  : "+TARGET_PATH;
  print "Output DEBUG path   : "+target_bin_path_DEBUG;
  print "Output RELEASE path : "+target_bin_path_RELEASE;

  
# Base libraries
print "|---------EXTERNAL: Windows 64a-----|";
if (clean == 1):
  syncPathWithDelete(os.path.join(THIS_PATH, "lib", "release64a", "*.*"), target_bin_path_RELEASE);
  syncPathWithDelete(os.path.join(THIS_PATH, "lib", "debug64a", "*.*"), target_bin_path_DEBUG);
else:
  syncPath(os.path.join(THIS_PATH, "lib", "release64a", "*.*"), target_bin_path_RELEASE);
  syncPath(os.path.join(THIS_PATH, "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

#ABase libraries
print "|---------EXTERNAL: ABase-----------|";
syncPath(os.path.join(BASE_PATH, "ALibrary", "ABase", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibrary", "ABase", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

# ACrypto
print "|---------EXTERNAL: ALibraryImpl/ACrypto-----------|";
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "ACrypto", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "ACrypto", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

# ADatabase_MySQL
print "|---------EXTERNAL: ALibraryImpl/ADatabase_MySQL-----------|";
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "ADatabase_MySQL", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "ADatabase_MySQL", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

# AZlib
print "|---------EXTERNAL: ALibraryImpl/AZlib---------------------|";
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AZlib", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AZlib", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

# AXsl
print "|---------EXTERNAL: ALibraryImpl/AXsl----------------------|";
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AXsl", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AXsl", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

# AXsl
print "|---------EXTERNAL: ALibraryImpl/AGdLib--------------------|";
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AGdLib", "lib", "release64a", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(BASE_PATH, "ALibraryImpl", "AGdLib", "lib", "debug64a", "*.*"), target_bin_path_DEBUG);

#SQLite3 tools
print "|---SQLite3 redistributable--------------------------------|";
syncPath(os.path.join(BASE_PATH, "_devtools", "sqlite3", "sqlite3.exe"), target_bin_path_DEBUG);
syncPath(os.path.join(BASE_PATH, "_devtools", "sqlite3", "sqlite3.exe"), target_bin_path_RELEASE);

# Copy build results if creating external distribution
if (os.path.join(target_bin_path_DEBUG, "foo") != os.path.join(DEBUG_INPUT_PATH, "foo")):
  # ALibrary DEBUG targets
  print "|-----DEBUG OUTPUT: ALibrary-------------------------------|";
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ABase.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ABase.lib"), target_bin_path_DEBUG);

  # ALibraryImpl DEBUG targets
  print "|-----DEBUG OUTPUT: ALibraryImpl---------------------------|";
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ACrypto.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ACrypto.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_ODBC.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_ODBC.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_MySQL.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_MySQL.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_SQLite.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_SQLite.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ALuaEmbed.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "ALuaEmbed.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AXsl.lib"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AXsl.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AZlib.dll"), target_bin_path_DEBUG);
  syncPath(os.path.join(DEBUG_INPUT_PATH, "AZlib.lib"), target_bin_path_DEBUG);

  # ALibrary RELEASE targets
  print "|---RELEASE OUTPUT: ALibrary-------------------------------|";
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ABase.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ABase.lib"), target_bin_path_RELEASE);

  # ALibraryImpl RELEASE targets
  print "|---RELEASE OUTPUT: ALibraryImpl---------------------------|";
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ACrypto.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ACrypto.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_ODBC.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_ODBC.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_MySQL.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_MySQL.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_SQLite.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_SQLite.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AGdLib.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AGdLib.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ALuaEmbed.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "ALuaEmbed.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AXsl.lib"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AXsl.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AZlib.dll"), target_bin_path_RELEASE);
  syncPath(os.path.join(RELEASE_INPUT_PATH, "AZlib.lib"), target_bin_path_RELEASE);
  
  #VC redistributable
  print "|---Visual C++ redistributable-----------------------------|";
  syncPath(os.path.join(THIS_PATH, "vc2008_redist", "vcredist_64a_vc2008.exe"), os.path.join(TARGET_PATH, "_dist_windows"));
  
print "|-----------TOUCH MARKERS----------------------------------|";
touch(os.path.join(target_bin_path_DEBUG, "___DEBUG_OUTPUT___"));
touch(os.path.join(target_bin_path_RELEASE, "___RELEASE_OUTPUT___"));
