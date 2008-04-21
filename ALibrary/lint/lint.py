#!/bin/python
import os;
import sys;

def makeSystemCall(param):
	if (verbose == 1):
		print param;
	os.system(param);
	 
def makeSystemCall_mkdir(dirname):
	if (not os.path.exists(dirname)):
		CMD = "mkdir \""+dirname+"\"";
		if (verbose == 1):
			print CMD;
		os.system(CMD);


verbose = 0;
clean = 0;
argc = 1;
while (len(sys.argv) > argc):
	if (sys.argv[argc] == "clean"):
		clean = 1;
	elif (sys.argv[argc] == "verbose"):
		verbose = 1;
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

flexe = "flexelint.exe "
ARGS = "ALibrary.lnt"

makeSystemCall(flexe+ARGS);
