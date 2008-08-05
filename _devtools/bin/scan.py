#!/usr/bin/python
#
# Scan current project
#
import os,sys;

def showUsage():
  print "Usage: "+os.path.basename(sys.argv[0])+" [options]";
  print "\r\nOptions";
  print "-p <path>      - base path to scan, if omitted current path is used";
  print "-verbose       - verbose output";
  print "-help";

THIS_DIR=os.path.dirname(sys.argv[0]);
TARGET_PATH=os.getcwd();
TARGET_FILE="";
verbose = 0;
argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-verbose"):
    verbose = 1;
  elif (sys.argv[argc] == "-p"):
    TARGET_PATH = sys.argv[argc+1];
    argc += 1;
  elif (sys.argv[argc] == "-f"):
    TARGET_FILE = sys.argv[argc+1];
    argc += 1;
  elif (sys.argv[argc] == "-help"):
    showUsage();
    sys.exit(-1);
  
  argc += 1;

SCAN_ASW_SCRIPT = os.path.join(THIS_DIR, "scan_ASW.py");
SCAN_INCLUDE_SCRIPT = os.path.join(THIS_DIR, "scan_include.py");

if (verbose != 0):
  print "Verbose mode enabled";
  print "TARGET_PATH="+TARGET_PATH;
  print "SCAN_ASW_SCRIPT="+SCAN_ASW_SCRIPT;
  print "SCAN_INCLUDE_SCRIPT="+SCAN_INCLUDE_SCRIPT;

def processDirectory(fullpath, targets):
  if (os.path.split(fullpath)[1] == ".svn"):
    if (verbose != 0):
      print "  Ignoring .svn: "+fullpath;
    return 0;
  
  if (verbose != 0):
    print "+++Directory detected, adding: "+fullpath;
  
  targets.append(fullpath);
  return 1;
  
def getAllFilesInSubdirectory(basepath, extension, targets):
  if (processDirectory(basepath, targets) == 0):
    return;

  files = os.listdir(basepath);
  for file in files:
    fullpath = os.path.join(basepath,file);
    if (verbose != 0):
      print "  Found: "+fullpath;
    if (os.path.isdir(fullpath)):
      getAllFilesInSubdirectory(fullpath, extension, targets);
    else:
      if (verbose != 0):
        print "  Ignoring: "+fullpath;
  return;

def readTargetFile(file, targets):
  f = open(file,"r");
  for dirname in f.readlines():
    dirname = dirname.strip(" \r\n\t");
    print "Processing: "+dirname;
    getAllFilesInSubdirectory(dirname, "cpp", targets);
  return;

# Gather all C++ files (*.cpp) in all directories
targets = [];
if (TARGET_FILE != ""):
  readTargetFile(TARGET_FILE, targets);
else:
  getAllFilesInSubdirectory(TARGET_PATH, "cpp", targets);

print "Processing "+str(len(targets))+" files.";
for target in targets:
  if (verbose != 0):
    print "ASW processing: "+target;
  os.system(SCAN_ASW_SCRIPT+" "+target);
  if (verbose != 0):
    print "include processing: "+target;
  os.system(SCAN_INCLUDE_SCRIPT+" "+target);
