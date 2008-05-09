#!/bin/python
import os,sys;
 
def makeSystemCall(param):
	print param;
	os.system(param);
	 
def makeSystemCall_mkdir(dirname):
	if (not os.path.exists(dirname)):
		CMD = "mkdir \""+dirname+"\"";
		print CMD;
		os.system(CMD);

def makeSystemCall_copy(cfrom, cto):
	CMD = "copy \""+cfrom+"\" \""+cto+"\"";
	print CMD;
	os.system(CMD);

def makeSystemCall_copydir(cfrom, cto):
	makeSystemCall_mkdir(cto);
	CMD = "xcopy /IY \""+cfrom+"\" \""+cto+"\"";
	os.system(CMD);

def makeSystemCall_copydir_recursive(cfrom, cto):
	CMD= "xcopy /SIY \""+cfrom+"\" \""+cto+"\"";
	print CMD;
	os.system(CMD);

def makeSystemCall_remove(filename):
	CMD = "del \""+filename+"\"";
	print CMD;
	os.system(CMD);


######################################################################
########################## MAIN ######################################
######################################################################

TARGET_PATH = os.sep+"aos";

if (len(sys.argv) > 1):
	TARGET_PATH = sys.argv[1];

if (TARGET_PATH == "-help" or TARGET_PATH == "?"):
	print "Usage: "+sys.argv[0]+" <target path>";
	print " <target path> = where everything is copied to";
	sys.exit(0);

base_path = os.path.split(os.path.join(os.getcwd()))[0];

# Gather ALibrary distribution
os.chdir(os.path.join(base_path, "ALibrary"));
makeSystemCall("gather_distribution.py "+TARGET_PATH);
os.chdir(os.path.join(base_path, "AObjectServer"));

# AObjectServer and subprojects
print "----------AObjectServer base--------";
makeSystemCall_mkdir(os.path.join(TARGET_PATH, "AObjectServer"));
makeSystemCall_copydir(os.path.join(base_path, "AObjectServer", "AObjectServer", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer"));
makeSystemCall_copy(os.path.join(base_path, "AObjectServer", "AObjectServer.sln"), os.path.join(TARGET_PATH, "AObjectServer"));
makeSystemCall_copy(os.path.join(base_path, "AObjectServer", "GETTING_STARTED.txt"), TARGET_PATH);
makeSystemCall_copy(os.path.join(base_path, "AObjectServer", "FAQ.html"), TARGET_PATH);

print "----------AObjectServer--------";
makeSystemCall_copydir(os.path.join(base_path, "AObjectServer", "AObjectServer", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AObjectServer"));

print "----------AOS_Base--------";
makeSystemCall_copydir(os.path.join(base_path, "AObjectServer", "AOS_Base", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOS_Base"));

print "----------AOS_BaseModules--------";
makeSystemCall_copydir(os.path.join(base_path, "AObjectServer", "AOS_BaseModules", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOS_BaseModules"));

print "----------AOSWatchDog--------";
makeSystemCall_copydir(os.path.join(base_path, "AObjectServer", "AOSWatchDog", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "AOSWatchDog"));

print "----------aosconfig--------";
makeSystemCall_copydir_recursive(os.path.join(base_path, "AObjectServer", "aosconfig", "*.*"), os.path.join(TARGET_PATH, "AObjectServer", "aosconfig"));

print "----------Scripts--------";
makeSystemCall_copy(os.path.join(base_path, "AObjectServer", "rsync.py"), os.path.join(TARGET_PATH, "AObjectServer"));
makeSystemCall_copy(os.path.join(base_path, "AObjectServer", "make_aosconfig.py"), os.path.join(TARGET_PATH, "AObjectServer"));

# AOS_Modules
print "----------AOS_Modules--------";
makeSystemCall_copydir_recursive(os.path.join(base_path, "AOS_Modules", "*.*"), os.path.join(TARGET_PATH, "AOS_Modules"));

# Cleanup and finalize
makeSystemCall_remove(os.path.join(TARGET_PATH, "AOS_Modules", "AOS_Modules.*"));
