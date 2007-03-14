#!/bin/python

import os,sys;

def makeSystemCall(param):
	os.system(param);

def showUsage():
	print "Usage: "+sys.argv[0]+" <debug|release|all> <build|rebuild|clean>";
	print "Default: debug build";

############################################################################################
################################### MAIN ###################################################
############################################################################################

buildtype = "debug";
if (len(sys.argv) > 1):
	buildtype = sys.argv[1];

buildmode = "build";
if (len(sys.argv) > 2):
	buildmode = sys.argv[2];

if (buildtype != "debug" and buildtype != "release" and buildtype != "all"):
	showUsage();
	sys.exit(-1);

if (buildmode != "build" and buildmode != "rebuild" and buildmode != "clean"):
	showUsage();
	sys.exit(-1);


makeSystemCall("..\\ALibrary\\gather_headers_only.py");

if (buildtype == "debug" or buildtype == "all"):
	print "|------DEBUG BUILD: ALibrary-------------------------------|";
	makeSystemCall("devenv AObjectServer.71.sln /"+buildmode+" Debug /project AObjectServer");

if (buildtype == "release" or buildtype == "all"):
	print "|----RELEASE BUILD: ALibrary-------------------------------|";
	makeSystemCall("devenv AObjectServer.71.sln /"+buildmode+" Release /project AObjectServer");

