#!/usr/bin/python
#
# Synchronize content
#
import os,sys;

THIS_PATH=os.path.dirname(sys.argv[0]);
BASE_ENV_PATH=os.path.normpath(os.path.join(THIS_PATH,"..","_devtools","bin"));
def makeSystemCall(param):
  if (verbose == 1):
    print "Executing: "+param;
  if (dryrun == 0):
    os.system(param);

# rsync.py base command
RSYNC_BASE=BASE_ENV_PATH+os.sep+"rsync.py -rtuC --exclude=.svn ";
def syncModule(module_source_path, targetpath):
  EXEC_RSYNC_BASE = RSYNC_BASE + " ";
  if (dryrun == 1):
    EXEC_RSYNC_BASE += "-n ";
  
  EXEC_RSYNC_BASE += module_source_path + " ";	

  print "|----------------------------Content: "+module_source_path+" -> "+targetpath;
  EXEC_RSYNC = EXEC_RSYNC_BASE + targetpath;
  if (verbose == 1):
    print EXEC_RSYNC;
  os.system(EXEC_RSYNC);

def sync_aos_root(module_path, targetpath):
  # Now process modules located in module_path
  # (similar code can be added for other module paths by calling syncModule(...source path...)
  parent_path = os.path.split(os.getcwd())[0];
  local_modules_path = os.path.join(parent_path, module_path);
  if (os.path.exists(local_modules_path)):
    modules = os.listdir(local_modules_path);
    for module in modules:
      module_path = os.path.join(local_modules_path, module);
      if (os.path.isdir(module_path)):
        module_source_path = os.path.join(module_path, "aos_root");
        if (os.path.exists(module_source_path)):
          syncModule(module_source_path, targetpath);

def showUsage():
  print "AOS configuration and content synchronization script.";
  print sys.argv[0]+" [-win32] [-win64] <-verbose|-dryrun|-clean|-makebear> <-p BASEPATH>";
  print "  -win32          - Win32 target"
  print "  -win64          - Win64 target"
  print "  -v              - verbose mode";
  print "  -clean          - removes previous version";
  print "  -dryrun         - only show what is going to be copied, but do not copy";
  print "  -makebare       - Create a bare version of aos_root in aos_root_bare"
  print "  -p BASEPATH     - uses BASEPATH/_debug/ and BASEPATH/_release for deployment";
  print "\r\nExample:";
  print " -v -p /output/dir/";
  print " -p /tmp dryrun";
  
######################################################################
########################## MAIN ######################################
######################################################################

source_path = os.path.join(os.getcwd(), "aos_root");

target_path = "..\\";
dryrun = 0;
verbose = 0;
argc = 1;
clean = 0;
do_win32 = 0;
do_win64 = 0;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-dryrun"):
    dryrun = 1;
  elif (sys.argv[argc] == "-v"):
    verbose = 1;
  elif (sys.argv[argc] == "-clean"):
    clean = 1;
  elif (sys.argv[argc] == "-win32"):
    do_win32 = 1;
  elif (sys.argv[argc] == "-win64"):
    do_win64 = 1;
  elif (sys.argv[argc] == "-p"):
    target_path = sys.argv[argc+1];
    argc += 1;
  else:
    print "Unknown parameter: "+sys.argv[argc];
    showUsage();
    sys.exit(-1);
  
  argc += 1;

if (do_win32 == 0 and do_win64 == 0):
  print "Please select -win32 and/or -win64";
  showUsage();
  sys.exit(-1);

target_path_support = os.path.join(target_path,"support");
target_path_DEBUG_32 = os.path.join(target_path,"_debug");
target_path_RELEASE_32 = os.path.join(target_path,"_release");
target_path_DEBUG_64 = os.path.join(target_path,"_debug64a");
target_path_RELEASE_64 = os.path.join(target_path,"_release64a");
 

# Now process modules located in ../AOS_Modules 
# (similar code can be added for other module paths by calling syncModule(...source path...)
parent_path = os.path.split(os.getcwd())[0];
if (verbose == 1):
  print "verbose="+str(verbose);
  print "dryrun="+str(dryrun);
  print "win32="+str(do_win32);
  print "win64="+str(do_win64);
  print "target_path="+target_path;
  print "target_path_support="+target_path_support;
  print "target_path_DEBUG_32="+target_path_DEBUG_32;
  print "target_path_RELEASE_32="+target_path_RELEASE_32;
  print "target_path_DEBUG_64="+target_path_DEBUG_64;
  print "target_path_RELEASE_64="+target_path_RELEASE_64;

  
# Clean if needed  
if (clean == 1):
  if (dryrun == 0):
    print "Cleaning target directory: "+target_path;
    makeSystemCall("del /Q /S "+os.path.join(target_path, "aos_root"));
  else:
    print "Dry-run detected, would clean: "+os.path.join(target_path, "aos_root");
    print "Exiting, rest of dry run cannot be determined with clean enabled";
    sys.exit(0);


# Copy extra needed stuff
if (not os.path.exists(target_path)):
  os.mkdir(target_path);

sync_aos_root("AObjectServer", target_path);
sync_aos_root("AOS_Modules", target_path);
syncModule("docs", target_path);

if (do_win32 == 1):
  syncModule("openssl_create_selfsigned_certificate.py", target_path_DEBUG_32);
  syncModule("openssl.config", target_path_DEBUG_32);
  syncModule("openssl_create_selfsigned_certificate.py", target_path_RELEASE_32);
  syncModule("openssl.config", target_path_RELEASE_32);
if (do_win64 == 1):
  syncModule("openssl_create_selfsigned_certificate.py", target_path_DEBUG_64);
  syncModule("openssl.config", target_path_DEBUG_64);
  syncModule("openssl_create_selfsigned_certificate.py", target_path_RELEASE_64);
  syncModule("openssl.config", target_path_RELEASE_64);
