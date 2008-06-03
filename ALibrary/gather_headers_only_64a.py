#!/bin/python
import os,sys;

BASE_ALIBRARY_PATH=os.path.dirname(sys.argv[0]);
BASE_ENV_PATH=os.path.join(BASE_ALIBRARY_PATH,"..","_devtools","bin");
if (not os.path.exists(BASE_ENV_PATH)):
  print "Unable to find rsync.py at "+BASE_ENV_PATH;
  sys.exit(-1);
RSYNC_SCRIPT = os.path.join(BASE_ENV_PATH,"rsync.py");
EXEC_RSYNC_BASE=RSYNC_SCRIPT+" -tuC --exclude=CVS --exclude=pch*.hpp ";

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
  CMD = EXEC_RSYNC_BASE + "--delete " + source_path + " " + target_path; 
  if (verbose == 1):
    print CMD;
  os.system(CMD);

def makeSystemCall_remove(filename):
  CMD = "del /q \""+filename+"\"";
  if (verbose == 1):
    print CMD;
  os.system(CMD);

def showUsage():
  print "Usage: "+sys.argv[0]+" [options]";
  print "-p <target path>   - Will create 'ALibrary/include64' directory and gather all pertinent headers";
  print "-clean             - Clean first";
  print "-v                 - Verbose mode";

######################################################################
########################### MAIN #######################################
######################################################################

INPUT_BASE_PATH = os.path.split(BASE_ALIBRARY_PATH)[0];
TARGET_BASE_PATH = os.path.split(BASE_ALIBRARY_PATH)[0];

verbose = 0;
clean = 0;
argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-clean"):
    clean = 1;
  elif (sys.argv[argc] == "-v"):
    verbose = 1;
  elif (sys.argv[argc] == "-p"):
    TARGET_BASE_PATH = sys.argv[argc+1];
    argc += 1;
  else:
    print "Unknown parameter: "+sys.argv[argc];
    showUsage();
    sys.exit(-1);
  
  argc += 1;

if (TARGET_BASE_PATH == "/?" or TARGET_BASE_PATH == "?"):
  showUsage();
  sys.exit(0);

TARGET_ALIBRARY_INCLUDE_PATH = os.path.join(TARGET_BASE_PATH, "include64a");

# Target paths
makeSystemCall_mkdir(TARGET_BASE_PATH);

# Create or clean include path
if (not os.path.exists(TARGET_ALIBRARY_INCLUDE_PATH)):
  makeSystemCall_mkdir(TARGET_ALIBRARY_INCLUDE_PATH);
elif (clean == 1):
  makeSystemCall_remove(os.path.join(TARGET_ALIBRARY_INCLUDE_PATH, "*.*"));

# ALibrary include
print "|----------HEADERS: ALibrary/ABase-------------------------|";
if (clean == 1):
  syncPathWithDelete(os.path.join(INPUT_BASE_PATH, "ALibrary", "ABase", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
else:
  syncPath(os.path.join(INPUT_BASE_PATH, "ALibrary", "ABase", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_MySQL
print "|----------HEADERS: ALibraryImpl/ADatabase_MySQL-----------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ADatabase_MySQL", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ADatabase_MySQL", "include64a", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_SQLite
print "|----------HEADERS: ALibraryImpl/ADatabase_SQLite----------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ADatabase_SQLite", "*.hpp"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ADatabase_SQLite", "sqlite3.h"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_ODBC
print "|----------HEADERS: ALibraryImpl/ADatabase_ODBC------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ADatabase_ODBC", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# APythonEmbed
print "|----------HEADERS: ALibraryImpl/APythonEmbed--------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "APythonEmbed", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ALuaEmbed
print "|----------HEADERS: ALibraryImpl/ALuaEmbed-----------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ALuaEmbed", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ALuaEmbed", "lua-5.1.3", "src", "*.h"), TARGET_ALIBRARY_INCLUDE_PATH);

# AXsl
print "|----------HEADERS: ALibraryImpl/AXsl----------------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "AXsl", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# AZlib
print "|----------HEADERS: ALibraryImpl/AZlib---------------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "AZlib", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ACrypto
print "|----------HEADERS: ALibraryImpl/ACrypto-------------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ACrypto", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "ACrypto", "openssl", "*.h*"), os.path.join(TARGET_ALIBRARY_INCLUDE_PATH, "openssl"));

# AGdLib
print "|----------HEADERS: ALibraryImpl/AGdLib--------------------|";
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "AGdLib", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(INPUT_BASE_PATH, "ALibraryImpl", "AGdLib", "libgd", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
