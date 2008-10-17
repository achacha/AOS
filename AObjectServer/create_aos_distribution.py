#!/bin/python
import os,sys;
 
THIS_PATH=os.path.dirname(sys.argv[0]);
BASE_PATH = os.path.normpath(os.path.join(THIS_PATH,".."));
EXEC_RSYNC_BASE="rsync -rtuC --exclude=.svn --exclude=*.user ";
EXEC_RSYNC_BASE_WITH_DELETE="rsync -rtuC --delete --exclude=.svn --exclude=*.user ";

def makeSystemCall(param):
	print param;
	os.system("cmd /C "+param);

def makeSystemCall_remove(filename):
	CMD = "del \""+filename+"\"";
	print CMD;
	os.system(CMD);

def syncPath(source_path, target_path):
  CMD = "";
  if (clean == 1):
    CMD = EXEC_RSYNC_BASE_WITH_DELETE + source_path + " " + target_path;
  else:
    CMD = EXEC_RSYNC_BASE + source_path + " " + target_path; 
  if (verbose == 1):
    print CMD;
  os.system(CMD);

def showUsage():
  print "Usage: "+sys.argv[0]+" [options]";
  print " -win32             - Create Win32 distro"
  print " -win64             - Create Win64 distro"
  print " -p <target path>   - target path = where everything is copied to";
  print " -v                 - verbose mode";
  print " -clean             - clean first";
  print " -help              - this help";

def makeAOSConfig(TARGET_PATH):
  CMD = "make_aosconfig.py -p "+TARGET_PATH;
  if (verbose == 1):
    CMD += " -v";
  if (clean == 1):
    CMD += " -clean";
  if (do_win32 == 1):
    CMD += " -win32";
  if (do_win64 == 1):
    CMD += " -win64";
  makeSystemCall(CMD);

def gatherDistribution(TARGET_PATH, base_path):
  if (do_win32 == 1):
    os.chdir(os.path.join(base_path, "_dist_windows", "x86"));
    CMD = "gather_distribution_x86.py -p "+os.path.join(TARGET_PATH);
    if (verbose == 1):
      CMD += " -v";
    if (clean == 1):
      CMD += " -clean";
    makeSystemCall(CMD);
    os.chdir(os.path.join(base_path, "AObjectServer"));

  if (do_win64 == 1):
    os.chdir(os.path.join(base_path, "_dist_windows", "a64"));
    CMD = "gather_distribution_64a.py -p "+os.path.join(TARGET_PATH);
    if (verbose == 1):
      CMD += " -v";
    if (clean == 1):
      CMD += " -clean";
    makeSystemCall(CMD);
    os.chdir(os.path.join(base_path, "AObjectServer"));
  
def gatherHeaders(TARGET_PATH, base_path):
  os.chdir(os.path.join(base_path, "ALibrary"));
  CMD = "call_gather_headers.py -p "+os.path.join(TARGET_PATH);
  if (verbose == 1):
    CMD += " -v";
  if (clean == 1):
    CMD += " -clean";
  if (do_win32 == 1):
    CMD += " -win32";
  if (do_win64 == 1):
    CMD += " -win64";
  makeSystemCall(CMD);
  os.chdir(os.path.join(base_path, "AObjectServer"));

  ######################################################################
########################## MAIN ######################################
######################################################################

TARGET_PATH = os.sep+"aos";

verbose = 0;
clean = 0;
argc = 1;
do_win32 = 0;
do_win64 = 0;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-clean"):
    clean = 1;
  elif (sys.argv[argc] == "-v"):
    verbose = 1;
  elif (sys.argv[argc] == "-win32"):
    do_win32 = 1;
  elif (sys.argv[argc] == "-win64"):
    do_win64 = 1;
  elif (sys.argv[argc] == "-p"):
    TARGET_PATH = sys.argv[argc+1];
    argc += 1;
  elif (sys.argv[argc] == "-help"):
    showUsage();
    sys.exit(-1);
  else:
    print "Unknown parameter: "+sys.argv[argc];
    showUsage();
    sys.exit(-1);
  
  argc += 1;

if (do_win32 == 0 and do_win64 == 0):
  print "Please specify -win32 and/or -win64.";
  showUsage();
  sys.exit(-1);

base_path = os.path.split(os.path.join(os.getcwd()))[0];
if (not os.path.exists(os.path.join(TARGET_PATH, "_devtools", "bin"))):
  os.makedirs(os.path.join(TARGET_PATH, "_devtools", "bin"));
syncPath(os.path.join(base_path, "_devtools", "bin", "rsync.py"), os.path.join(TARGET_PATH, "_devtools", "bin"));

# Gather ALibrary distribution
gatherDistribution(TARGET_PATH, base_path);

# Gather headers
gatherHeaders(TARGET_PATH, base_path);

# Make AOS configuration
makeAOSConfig(TARGET_PATH);

# AObjectServer and subprojects
print "----------AObjectServer base--------";
if (not os.path.exists(os.path.join(TARGET_PATH, "AObjectServer"))):
  os.makedirs(os.path.join(TARGET_PATH, "AObjectServer"));
syncPath(os.path.join(base_path, "AObjectServer", "AObjectServer", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer"));
syncPath(os.path.join(base_path, "AObjectServer", "GETTING_STARTED.txt"), TARGET_PATH);
syncPath(os.path.join(base_path, "AObjectServer", "CHANGELOG.txt"), TARGET_PATH);
syncPath(os.path.join(base_path, "AObjectServer", "LICENSE.txt"), TARGET_PATH);
syncPath(os.path.join(base_path, "AObjectServer", "CONTRIBUTORS.txt"), TARGET_PATH);
syncPath(os.path.join(base_path, "AObjectServer", "AObjectServerWithoutALibrary.sln"), os.path.join(TARGET_PATH, "AObjectServer"));
if (os.path.exists(os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer.sln"))):
  os.remove(os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer.sln"));
os.rename(os.path.join(TARGET_PATH, "AObjectServer", "AObjectServerWithoutALibrary.sln"), os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer.sln"))

print "----------AObjectServer--------";
syncPath(os.path.join(base_path, "AObjectServer", "AObjectServer", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer"));

print "----------AOS_Base--------";
syncPath(os.path.join(base_path, "AObjectServer", "AOS_Base", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOS_Base"));

print "----------AOS_BaseModules--------";
syncPath(os.path.join(base_path, "AObjectServer", "AOS_BaseModules", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOS_BaseModules"));

print "----------AOSWatchDog--------";
syncPath(os.path.join(base_path, "AObjectServer", "AOSWatchDog", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOSWatchDog"));

print "----------aosconfig--------";
syncPath(os.path.join(base_path, "AObjectServer", "aos_root", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "aos_root"));

# AOS_Modules
print "----------AOS_Modules--------";
if (not os.path.exists(os.path.join(TARGET_PATH, "AOS_Modules"))):
  os.makedirs(os.path.join(TARGET_PATH, "AOS_Modules"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_Classified", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Classified"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_Example", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Example"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_Security", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Security"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_Test", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Test"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_User", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_User"));
syncPath(os.path.join(base_path, "AOS_Modules", "AOS_Wiki", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Wiki"));
syncPath(os.path.join(base_path, "AOS_Modules", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules"));
makeSystemCall_remove(os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Modules.*"));
os.remove(os.path.join(TARGET_PATH, "AOS_Modules", "lint.cmd"));

# Doxygen help
print "---------docs----------------";
syncPath(os.path.join(base_path, "AObjectServer", "docs"), os.path.join(TARGET_PATH));
syncPath(os.path.join(base_path, "_doxygen", "AObjectServer", "html"), os.path.join(TARGET_PATH, "docs", "AObjectServer_doxygen"));
os.remove(os.path.join(TARGET_PATH, "docs", "docs.vcproj"));
