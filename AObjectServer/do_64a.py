#!/usr/bin/python
#
# Synchronize content
#
import os,sys;

THIS_PATH=os.path.dirname(sys.argv[0]);
ALIBRARY_PATH=os.path.normpath(os.path.join(THIS_PATH,"..","ALibrary"));
GATHER_SCRIPT=os.path.join(ALIBRARY_PATH,"gather_headers_only_64a.py");
MAKEAOS_SCRIPT="make_aosconfig_64a.py";

if (not os.path.exists(GATHER_SCRIPT)):
  print "Gather headers script missing: "+GATHER_SCRIPT;
  sys.exit(-1);

os.system(GATHER_SCRIPT);
os.system(MAKEAOS_SCRIPT);

