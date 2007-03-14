#!/bin/python
#

import os,sys;

def makeSystemCall(param):
	if (verbose == 1):
		print param;
	os.system(param);

output_path = "..\\";

def showUsage():
	print "Usage: "+sys.argv[0]+" <alternate local output path> [verbose | clean]";
	print "Default: output is to .. (parent directory)";
	
############################################################################################
################################### MAIN ###################################################
############################################################################################

verbose = 0;
clean = 0;
argc = 1;
TARGET_PATH = "../_debug";
while (len(sys.argv) > argc):
	if (sys.argv[argc] == "clean"):
		clean = 1;
	elif (sys.argv[argc] == "verbose"):
		verbose = 1;
	elif (argc == 1):
		TARGET_PATH = sys.argv[1];
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

if (TARGET_PATH == "/?" or TARGET_PATH == "?"):
	showUsage();
	sys.exit(0);

# Create local _debug and _release
print "|=================================================CREATE LOCAL ENVIRONMENT===|";
CMD = "gather_distribution.py";
if (verbose == 1):
	CMD += " verbose";
if (clean == 1):
	CMD += " clean";
makeSystemCall(CMD);

# Build deliverables
print "|==================================================BUILD LOCAL ENVIRONMENT===|";
CMD = "build_distribution.py";
CMD += " all";
if (clean == 1):
	CMD += " rebuild";
makeSystemCall(CMD);

# Create target _debug and _release with build output
print "|=======================================================CREATE DISTRIBUTION===|";
CMD = "gather_distribution.py "+output_path;
if (verbose == 1):
	CMD += " verbose";
if (clean == 1):
	CMD += " clean";
makeSystemCall(CMD);
