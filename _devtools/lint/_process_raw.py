#!/bin/python
#
# Process lint output
#

import os;
import sys;
import re;

class LintError:
  errno = "0";
  filename = "None";
  lineno = "";
  colno = "";
  function = "";
  description = "";
  def __init__(this, line):
    if (line[0:8] == "!!!Error"):
      # !!!Error[errno],filename:lineno:colno,function,description
      m = re.match("\[([0-9]+)\]\,([^,]+)\,([^,]+)\,([^,]+)\,([^,]*?)\,(.+)$",line[8:]);
      if (m != None):
        #print "m[1]="+m.group(1);
        #print "m[2]="+m.group(2);
        #print "m[3]="+m.group(3);
        #print "m[4]="+m.group(4);
        #print "m[5]="+m.group(5);
        #print "m[6]="+m.group(6);
        this.errno = m.group(1);
        this.filename = m.group(2);
        this.lineno = m.group(3);
        this.colno = m.group(4);
        this.function = m.group(5);
        this.description = m.group(6);
      else:
        if (1 == VERBOSE):
          print "No match!";
        this.errno = "Invalid";
        description = line;
  
  def __str__(this):
    return "!!!Error["+this.errno+"]," + this.filename + "," + this.lineno + "," + this.colno+"," + this.function+"," + this.description;

  def isValid(this):
    if ("Invalid" == this.errno):
      return 0;
    if (-1 != this.filename.find("\\Microsoft Visual Studio 9.0\\")):
      return 0;
    if (-1 != this.filename.find("\\Microsoft SDKs\\")):
      return 0;
    
    return 1;
    
def processError(line, fout):
  lerr = LintError(line);
  if (1 == lerr.isValid()):
    print >>fout,lerr;
    if (1 == VERBOSE):
      print "Adding: "+str(lerr);

def processLine(line, fout):
  if (line[0:8] == "!!!Error"):
    processError(line, fout);
  else:
    if (1 == VERBOSE):
      print "Non-error: "+line;

if (len(sys.argv) < 3):
  print "Usage: this <.raw input file> <output file after processing>";
  sys.exit(-1);

INPUT_FILE = sys.argv[1];
OUTPUT_FILE = sys.argv[2];
VERBOSE = 0;

print "Input : "+INPUT_FILE;
print "Output: "+OUTPUT_FILE;

# Backup existing output file
if (os.path.exists(OUTPUT_FILE+".bak")):
  os.remove(OUTPUT_FILE+".bak");
if (os.path.exists(OUTPUT_FILE)):
  print "Backing up existing output file";
  os.rename(OUTPUT_FILE, OUTPUT_FILE+".bak");
  
# Open input and output files
fout = open(OUTPUT_FILE, "w");
fin = open(INPUT_FILE, "r");

print;

current_module = "None";
done = 0;
line = fin.readline();
while (len(line) > 0):
  processLine(line, fout);
  
  if (line[0:11] == "--- Module:"):
    current_module = line[11:];
    current_module = current_module.strip();
    print "Processing: "+current_module;
    print >>fout,"---Module: "+current_module;

  line = fin.readline();

fin.close();
fout.close();
