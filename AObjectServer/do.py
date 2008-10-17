#!/usr/bin/python
#
# Synchronize content
#
import os,sys;

THIS_PATH=os.path.dirname(sys.argv[0]);
ALIBRARY_PATH=os.path.normpath(os.path.join(THIS_PATH,"..","ALibrary"));
GATHER_SCRIPT=os.path.join(ALIBRARY_PATH,"call_gather_headers.py");
MAKEAOS_SCRIPT=os.path.join(THIS_PATH, "call_make_aos_root.py");

if (not os.path.exists(GATHER_SCRIPT)):
  print "Gather headers script missing: "+GATHER_SCRIPT;
  sys.exit(-1);

if (not os.path.exists(MAKEAOS_SCRIPT)):
  print "Gather aos_root script missing: "+MAKEAOS_SCRIPT;
  sys.exit(-1);

os.system(GATHER_SCRIPT+" -win32");
os.system(MAKEAOS_SCRIPT+" -win32");

