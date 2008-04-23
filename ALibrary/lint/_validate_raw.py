#!/bin/python
#
# Validate lint output
# INPUT: basename (actual source file) of the file to be analyzed
#

import os;
import sys;

if (len(sys.argv) < 2):
  print "Usage: this_script.py <raw lint output file>\n";
  sys.exit(-1);
  
basefile = sys.argv[1];

print "Validating lint output: "+basefile; 

if (not os.path.exists(basefile)):
  print "  File \'"+basefile+"\' does not exist.";
  sys.exit(-2);

error_count = 0;
f = open(basefile, 'r');
for line in f.xreadlines():
  em = line.find("  Unable to open include file");
  if (em != -1):
    s = line.find("\'", em);
    e = line.find("\'", s+1);
    print "  Cannot include file: "+line[s+1:e];
    error_count += 1;

  em = line.find("  Unable to open module");
  if (em != -1):
    s = line.find("\'", em);
    e = line.find("\'", s+1);
    print "  Cannot include module: "+line[s+1:e];
    error_count += 1;

  em = line.find("#error ");
  if (em == 0):
    "  Preprocessor error: "+line[:-1];
    error_count += 1;

f.close();

if (error_count > 0):
  print "  Detected "+str(error_count)+" errors";
else:
  print "  No errors detected";

sys.exit(error_count);
