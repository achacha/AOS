#!/usr/bin/python
#
# Synchronize content
#
import os,sys;
from acommon import *;

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
          context.syncAosRoot(base_path);
        else:
          sync_aos_root(context, module_path);

######################################################################
########################## MAIN ######################################
######################################################################

context = Context(os.getcwd());
if (context.processArgv(sys.argv) == -1 or context.hasNoPlatform()):
  print("Please select -win32 and/or -win64 and/or -linux32 and/or -linux64");
  context.showUsage();
  sys.exit(-1);

context.TARGET_AOS_ROOT_PATH = os.path.join(context.TARGET_PATH, "aos_root");

# Clean if needed
if (context.clean == 1):
  if (context.dryrun == 1):
    print("Dry-run detected, would clean: "+context.TARGET_AOS_ROOT_PATH);
    print("Exiting, rest of dry run cannot be determined with clean enabled");
    sys.exit(0);
  else:
    # Remove aos_root directory
    print("Removing: "+context.TARGET_AOS_ROOT_PATH);
    os.system("rm -Ir "+ context.TARGET_AOS_ROOT_PATH);

# Copy extra needed stuff
if (not os.path.exists(context.TARGET_PATH)):
  os.mkdir(context.TARGET_PATH);

# Sync aos_root directories
print("Sync AObjectServer aos_root")
sync_aos_root(context, context.BASE_AOBJECTSERVER_PATH);
print("Sync AOS_Modules aos_root")
sync_aos_root(context, context.BASE_AOSMODULES_PATH);

# Docs
print("Sync Docs: "+os.path.join(context.BASE_AOBJECTSERVER_PATH, "docs")+" -> "+context.TARGET_PATH);
context.syncPathRecursive(os.path.join(context.BASE_AOBJECTSERVER_PATH, "docs"), context.TARGET_PATH);

print("Sync SSL and AOSWatchDog config")
context.syncFileToOutput(os.path.join(context.BASE_AOBJECTSERVER_PATH, "openssl_create_selfsigned_certificate.py"));
context.syncFileToOutput(os.path.join(context.BASE_AOBJECTSERVER_PATH, "openssl.config"));
context.syncFileToOutput(os.path.join(context.BASE_AOBJECTSERVER_PATH, "AOSWatchDog", "AOSWatchDog.ini"));

context.fixPermissions(context.BASE_AOBJECTSERVER_PATH);
context.fixPermissionsRecursive(context.TARGET_AOS_ROOT_PATH);