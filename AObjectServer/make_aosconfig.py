#!/bin/python
import os,sys;

def makeSystemCall(param):
	os.system(param);

# rsync.py base command
RSYNC_BASE=".\\rsync.py -rtuC --exclude=.svn ";
def syncModule(module_source_path):
	EXEC_RSYNC_BASE = RSYNC_BASE + module_source_path + " ";
	if (dryrun == 1):
		EXEC_RSYNC_BASE += "-n ";
	
	print "|----------------------------DEBUG content: "+module_source_path;
	EXEC_RSYNC = EXEC_RSYNC_BASE + target_path_DEBUG; 
	if (verbose == 1):
		print EXEC_RSYNC;
	os.system(EXEC_RSYNC);

	print "|--------------------------RELEASE content: "+module_source_path;
	EXEC_RSYNC = EXEC_RSYNC_BASE + target_path_RELEASE; 
	if (verbose == 1):
		print EXEC_RSYNC;
	os.system(EXEC_RSYNC);


def showUsage():
	print "AOS configuration and content synchronization script.";
	print sys.argv[0]+" <verbose|dryrun> <-basepath BASEPATH>";
	print "  verbose - extra information";
	print "  dryrun - only show what is going to be copied, but do not copy";
	print "  -basepath BASEPATH - uses BASEPATH/_debug/ and BASEPATH/_release for deployment";
	print "\r\nExample:";
	print " verbose -basepath /output/dir/";
	print " -target /tmp dryrun";
	
######################################################################
########################## MAIN ######################################
######################################################################

source_path = os.path.join(os.getcwd(), "aos_root");

target_path = "..\\";
dryrun = 0;
verbose = 0;
argc = 1;
while (len(sys.argv) > argc):
	if (sys.argv[argc] == "dryrun"):
		dryrun = 1;
	elif (sys.argv[argc] == "verbose"):
		verbose = 1;
	elif (sys.argv[argc] == "-basepath"):
		target_path = sys.argv[argc+1];
		argc += 1;
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

target_path_DEBUG = os.path.join(target_path, "_debug");
target_path_RELEASE = os.path.join(target_path, "_release");

if (verbose == 1):
	print "verbose="+str(verbose);
	print "dryrun="+str(dryrun);
	print "target_path_DEBUG="+target_path_DEBUG;
	print "target_path_RELEASE="+target_path_RELEASE;

# Now process modules located in ../AOS_Modules 
# (similar code can be added for other module paths by calling syncModule(...source path...)
parent_path = os.path.split(os.getcwd())[0];
modules_path = os.path.join(parent_path, "AObjectServer");
if (os.path.exists(modules_path)):
	modules = os.listdir(modules_path);
	for module in modules:
		module_path = os.path.join(modules_path, module);
		if (os.path.isdir(module_path)):
			module_source_path = os.path.join(module_path, "aos_root");
			if (os.path.exists(module_source_path)):
				syncModule(module_source_path);

# Now process modules located in ../AOS_Modules 
# (similar code can be added for other module paths by calling syncModule(...source path...)
parent_path = os.path.split(os.getcwd())[0];
modules_path = os.path.join(parent_path, "AOS_Modules");
if (os.path.exists(modules_path)):
	modules = os.listdir(modules_path);
	for module in modules:
		module_path = os.path.join(modules_path, module);
		if (os.path.isdir(module_path)):
			module_source_path = os.path.join(module_path, "aos_root");
			if (os.path.exists(module_source_path)):
				syncModule(module_source_path);
