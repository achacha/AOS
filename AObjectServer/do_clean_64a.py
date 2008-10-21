#!/usr/bin/python
#
# Synchronize content
#
#!/bin/python
import os,sys;

devenvbin = os.path.normpath(os.path.join(os.getcwd(), "..", "_devtools", "bin"));
sys.path.append(devenvbin);
from acommon import *;

context = Context(devenvbin);
if (context.processArgv(sys.argv) == -1):
  showUsage();
  sys.exit(-1);

context.win64 = 1;
context.clean = 1;
  
# Gather headers
context.executeScript("gather_headers.py");

# Make aos_root
context.executeScript("make_aos_root.py");
