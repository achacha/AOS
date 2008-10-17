#!/usr/bin/python
#
# Synchronize content
#
import os,sys;
from acommon import *;

def syncModule(context, module_source_path):
  print("|----------------------------Content: "+module_source_path+" -> "+context.BASE_CODE_PATH);
  context.recursive = 1;
  syncPath(context, module_source_path, context.BASE_CODE_PATH);
  context.recursive = 0;

def syncDocs(context):
  source_path = os.path.join(context.BASE_AOBJECTSERVER_PATH, "docs");
  target_path = os.path.join(context.AOS_TARGET_PATH, "static");
  print("|----------------------------Docs: "+source_path+" -> "+target_path);
  context.recursive = 1;
  syncPath(context, source_path, target_path);
  context.recursive = 0;

def sync_aos_root(context, base_path):
  # Now process modules located in module_path
  # (similar code can be added for other module paths by calling syncModule(...source path...)
  if (context.verbose == 1):
    print("sync_aos_root("+base_path+")");
  if (os.path.exists(base_path)):
    for module in os.listdir(base_path):
      module_path = os.path.join(base_path, module);
      if (os.path.isdir(module_path)):
        if (module == "aos_root"):
          syncModule(context, module_path);
        else:
          sync_aos_root(context, module_path);

def showUsage():
  print("AOS configuration and content synchronization script.");
  print(sys.argv[0]+" [-win32] [-win64] [-linux] <-verbose|-dryrun|-clean|-makebear> <-p BASEPATH>");
  print("  -win32          - Windows 32-bit target");
  print("  -win64          - Windows 64-bit target");
  print("  -linux32        - Linux 32-bit target");
  print("  -linux64        - Linux 64-bit target");
  print("  -v              - verbose mode");
  print("  -clean          - removes previous version");
  print("  -dryrun         - only show what is going to be copied, but do not copy");
  print("  -makebare       - Create a bare version of aos_root in aos_root_bare");
  print("  -p BASEPATH     - uses BASEPATH/_debug/ and BASEPATH/_release for deployment");
  print("\r\nExample:");
  print(" -v -p /output/dir/");
  print(" -p /tmp -dryrun");

######################################################################
########################## MAIN ######################################
######################################################################

context = Context(os.getcwd());
context.AOS_TARGET_PATH = os.path.join(context.BASE_CODE_PATH, "aos_root");

argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-dryrun"):
    context.dryrun = 1;
  elif (sys.argv[argc] == "-v"):
    context.verbose = 1;
  elif (sys.argv[argc] == "-clean"):
    context.clean = 1;
  elif (sys.argv[argc] == "-win32"):
    context.win32 = 1;
  elif (sys.argv[argc] == "-win64"):
    context.win64 = 1;
  elif (sys.argv[argc] == "-linux32"):
    context.linux32 = 1;
  elif (sys.argv[argc] == "-linux64"):
    context.linux64 = 1;
  elif (sys.argv[argc] == "-p"):
    context.AOS_TARGET_PATH = sys.argv[argc+1];
    argc += 1;
  else:
    print("Unknown parameter: "+sys.argv[argc]);
    showUsage();
    sys.exit(-1);

  argc += 1;

if (context.hasNoPlatform()):
  print("Please select -win32 and/or -win64 and/or -linux32 and/or -linux64");
  showUsage();
  sys.exit(-1);

# Now process modules located in ../AOS_Modules
# (similar code can be added for other module paths by calling syncModule(...source path...)
if (context.verbose == 1):
  print(str(context));

# Clean if needed
if (context.clean == 1):
  if (context.dryrun == 1):
    print("Dry-run detected, would clean: "+os.path.join(target_path, "aos_root"));
    print("Exiting, rest of dry run cannot be determined with clean enabled");
    sys.exit(0);
  else:
    # Remove aos_root directory
    print("Removing: "+context.AOS_TARGET_PATH);
    os.system("rm -Ir "+ context.AOS_TARGET_PATH);

# Copy extra needed stuff
if (not os.path.exists(context.AOS_TARGET_PATH)):
  os.mkdir(context.AOS_TARGET_PATH);

# Sync aos_root directories
sync_aos_root(context, context.BASE_AOBJECTSERVER_PATH);
sync_aos_root(context, context.BASE_AOSMODULES_PATH);
syncDocs(context);

syncFileToOutput(context, os.path.join(context.BASE_AOBJECTSERVER_PATH, "openssl_create_selfsigned_certificate.py"));
syncFileToOutput(context, os.path.join(context.BASE_AOBJECTSERVER_PATH, "openssl.config"));

