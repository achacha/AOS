#!/bin/python
#
# Process lint output
#

import os;
import sys;

if (len(sys.argv) < 3):
  print "Usage: this <.raw input file> <output file after processing>";
  sys.exit(-1);

INPUT_FILE = sys.argv[1];
OUTPUT_FILE = sys.argv[2];

print "Input : "+INPUT_FILE;
print "Output: "+OUTPUT_FILE;

# Backup existing output file
if (os.path.exists(OUTPUT_FILE+".bak")):
  os.remove(OUTPUT_FILE+".bak");
if (os.path.exists(OUTPUT_FILE)):
  print "Backing up existing output file";
  os.rename(OUTPUT_FILE, OUTPUT_FILE+".bak");

sys.exit(0);
  
# Need to do something here I suppose
  
# Open input and output files
#fout = open(OUTPUT_FILE, "w");
#fin = open(INPUT_FILE, "r");

done = 0;
while (done == 0):
  line0 = fin.readline();
  if (len(line0) == 0):
    done = 1;
    line1 = fin.readline();
    if (len(line1) == 0):
      print "Unexpected EOF after: "+line0;
      sys.exit(-2);
    
    # Process line1, line0 contains the source
    # <full path>(lineno): <error|warning|etc> <number>: <description>
    print line1;

fin.close();
#fout.close();
