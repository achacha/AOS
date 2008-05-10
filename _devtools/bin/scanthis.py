#!/usr/bin/python
#
# Scan current project
#
import os,sys;

THIS_DIR=os.path.dirname(sys.argv[0]);
SCAN_SCRIPT=os.path.join(THIS_DIR,"..","devtools","bin","scan.py");

if (not os.path.exists(SCAN_SCRIPT)):
  print "Unable to find scan script: "+SCAN_SCRIPT;
  sys.exit(-1);

SCAN_CONF=os.path.join(THIS_DIR,"scan.conf");
if (os.path.exists(SCAN_CONF)):
  print "Using scan configuration: "+SCAN_CONF;
  os.system(SCAN_SCRIPT+" -f "+SCAN_CONF);
else:
  print "Using current directory as base path.";
  os.system(SCAN_SCRIPT+" -p "+THIS_DIR);
