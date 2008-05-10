#!/usr/bin/python
#
# Lint a project
#

import os,sys,stat;

CURRENT_DIR=os.getcwd();
THIS_DIR=os.path.dirname(sys.argv[0]);
os.chdir(THIS_DIR);

def isDirectory(path):
  return stat.S_ISDIR(os.stat(path)[stat.ST_MODE]);

def getCppFiles(files, basepath):
  if (verbose == 1):
    print "Processing: "+basepath;

  for f in os.listdir(basepath):
    if (isDirectory(os.path.join(basepath,f))):
      if (f != ".svn"):
        getCppFiles(files, os.path.join(basepath,f));
    else:
      if (f[-4:] == ".cpp"):
        files.append(os.path.join(basepath,f));
        if (verbose == 1):
          print "  +"+f;
      else:
        if (verbose == 1):
          print "  -"+f;
  return files;

def makeSystemCall(param):
  if (verbose == 1):
    print param;
  return os.system(param);

def makeSystemCall_mkdir(dirname):
  if (not os.path.exists(dirname)):
    CMD = "mkdir \""+dirname+"\"";
    if (verbose == 1):
      print CMD;
    return os.system(CMD);

def removeFile(filename):
  if (os.path.exists(filename)):
    os.remove(filename);
    if (verbose == 1):
      print "  Removing: "+filename;
  elif (verbose == 1):
    print "  Not removing, file does not exist: "+filename;
    
def showUsage():
  print "Usage: "+os.path.split(sys.argv[0])[1]+" <project base directory, all .cpp files in all directories will be analyzed> [clean | verbose | nolint]";
  

#================================================================================
#  
# MAIN
#     
PROJECT_PATH = "";
PROJECT = "";
verbose = 0;
clean = 0;
nolint = 0;
argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "clean"):
    clean = 1;
  elif (sys.argv[argc] == "verbose"):
    verbose = 1;
  elif (sys.argv[argc] == "nolint"):
    nolint = 1;
  else:
    if ("" == PROJECT):
      PROJECT_PATH = sys.argv[argc];
      if (not os.path.exists(PROJECT_PATH)):
        PROJECT_PATH = os.path.join(CURRENT_DIR,PROJECT_PATH); # try relative
        if (not os.path.exists(PROJECT_PATH)):
          print "Unable to find absolute nor relative path: "+PROJECT_PATH+" or "+sys.argv[argc];
          sys.exit(-1);
      PROJECT = os.path.normpath(os.path.join(CURRENT_DIR,PROJECT_PATH));
      PROJECT = PROJECT.replace(":", "_");
      PROJECT = PROJECT.replace(".", "_");
      PROJECT = PROJECT.replace("\\", "-");
      PROJECT = PROJECT.replace("//", "-");
      print "Project base directory: "+PROJECT_PATH;
      print "Project name: "+PROJECT;
    else:
      showUsage();
      sys.exit(-1);
  argc += 1;

if ("" == PROJECT):
  showUsage();
  sys.exit(-1);
  
flexe = "flexelint.exe "

BASE_OUTPUT_DIR = os.path.normpath(os.path.join(THIS_DIR,"..","..","..","_lint"));
LINT_CONFIG_FILE = os.path.join(BASE_OUTPUT_DIR, PROJECT+".lnt");
RAW_OUTPUT_FILE = LINT_CONFIG_FILE+".raw";
OUT_OUTPUT_FILE = LINT_CONFIG_FILE+".out.txt";

makeSystemCall_mkdir(BASE_OUTPUT_DIR);

if (clean == 1):
  print "Cleaning configuration and intermediate files.";
  removeFile(LINT_CONFIG_FILE);
  removeFile(RAW_OUTPUT_FILE);

#Create lint config file if needed
if (not os.path.exists(LINT_CONFIG_FILE)):
  if (not os.path.exists(PROJECT_PATH)):
    print "Unable to find: "+PROJECT_PATH;
    sys.exit(-2);
  
  print "Building lint configuration: "+LINT_CONFIG_FILE;
  
  flint = open(LINT_CONFIG_FILE, "wt");
  
  print "  Using _base.lnt";
  f = open("_base.lnt", "r");
  for line in f.xreadlines():
    flint.write(line);
  f.close();
  
  #Lint output information
  print "  Raw output: "+RAW_OUTPUT_FILE;
  print >>flint,"-os("+RAW_OUTPUT_FILE+")";
  
  print "  Adding all .cpp files in "+PROJECT_PATH;
  files = [];
  getCppFiles(files, PROJECT_PATH);
  print >>flint,"//==========FILES:START==========";
  for file in files:
    print >>flint,file;
  print >>flint,"//==========FILES:END==========";
  print "  Added "+str(len(files))+" files.";
  
  flint.close();  
else:
  print "Using existing lint configuration: "+LINT_CONFIG_FILE;
  
# Execute lint
if (0 == nolint):
  if (os.path.exists(RAW_OUTPUT_FILE+".bak")):
    os.remove(RAW_OUTPUT_FILE+".bak");
  if (os.path.exists(RAW_OUTPUT_FILE)):
    print "Backing up existing lint output";
    os.rename(RAW_OUTPUT_FILE, RAW_OUTPUT_FILE+".bak");
  
  print "===============Executing lint===============";
  makeSystemCall(flexe+LINT_CONFIG_FILE);
else:
  print "Skipping lint execution, assuming output exists";

if (not os.path.exists(RAW_OUTPUT_FILE)):
  print "ERROR: Lint did not generate raw output";
  sys.exit(-1);

# Validate results of lint to check if it failed or not
print "\r\n";
error_count = makeSystemCall("_validate_raw.py "+RAW_OUTPUT_FILE);
if (0 != error_count):
  print "Verification failed!";
  sys.exit(error_count);

# Process the results and extract only pertinent information
print "\r\n";
error_count = makeSystemCall("_process_raw.py "+RAW_OUTPUT_FILE+" "+OUT_OUTPUT_FILE);
if (0 != error_count):
  print "Processing failed!";
  sys.exit(error_count);
  
print "\r\nLint analysis complete\r\n  raw results: "+RAW_OUTPUT_FILE;
print "  processed results: "+OUT_OUTPUT_FILE;
