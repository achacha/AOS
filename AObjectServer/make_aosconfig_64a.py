#!/usr/bin/python
#
# Synchronize content
#
import os,sys;

THIS_PATH=os.path.dirname(sys.argv[0]);
BASE_ENV_PATH=os.path.normpath(os.path.join(THIS_PATH,"..","_devtools","bin"));
def makeSystemCall(param):
	if (verbose == 1):
		print "Executing: "+param;
	if (dryrun == 0):
		os.system(param);

# rsync.py base command
RSYNC_BASE=BASE_ENV_PATH+os.sep+"rsync.py -rtuC --exclude=.svn ";
def syncModule(module_source_path):
	EXEC_RSYNC_BASE = RSYNC_BASE + " ";
	if (dryrun == 1):
		EXEC_RSYNC_BASE += "-n ";
	
	EXEC_RSYNC_BASE += module_source_path + " ";	
	
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

def syncModule2(module_source_path, module_target_path):
	EXEC_RSYNC_BASE = RSYNC_BASE + " ";
	if (dryrun == 1):
		EXEC_RSYNC_BASE += "-n ";
	
	EXEC_RSYNC_BASE += module_source_path + " ";	
	
	print "|----------------------------DEBUG content: "+module_source_path;
	EXEC_RSYNC = EXEC_RSYNC_BASE + os.path.join(target_path_DEBUG, module_target_path); 
	if (verbose == 1):
		print EXEC_RSYNC;
	os.system(EXEC_RSYNC);

	print "|--------------------------RELEASE content: "+module_source_path;
	EXEC_RSYNC = EXEC_RSYNC_BASE + os.path.join(target_path_RELEASE, module_target_path); 
	if (verbose == 1):
		print EXEC_RSYNC;
	os.system(EXEC_RSYNC);

def showUsage():
	print "AOS configuration and content synchronization script.";
	print sys.argv[0]+" <-verbose|-dryrun|-clean> <-p BASEPATH>";
	print "  -v     - verbose mode";
	print "  -clean - removes previous version";
	print "  -dryrun - only show what is going to be copied, but do not copy";
	print "  -p BASEPATH - uses BASEPATH/_debug64a/ and BASEPATH/_release64a for deployment";
	print "\r\nExample:";
	print " -v -p /output/dir/";
	print " -p /tmp dryrun";
	
######################################################################
########################## MAIN ######################################
######################################################################

source_path = os.path.join(os.getcwd(), "aos_root");

target_path = "..\\";
dryrun = 0;
verbose = 0;
argc = 1;
clean = 0;
while (len(sys.argv) > argc):
	if (sys.argv[argc] == "-dryrun"):
		dryrun = 1;
	elif (sys.argv[argc] == "-v"):
		verbose = 1;
	elif (sys.argv[argc] == "-clean"):
		clean = 1;
	elif (sys.argv[argc] == "-p"):
		target_path = sys.argv[argc+1];
		argc += 1;
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

target_path_DEBUG = os.path.join(target_path, "_debug64a");
target_path_RELEASE = os.path.join(target_path, "_release64a");

# Now process modules located in ../AOS_Modules 
# (similar code can be added for other module paths by calling syncModule(...source path...)
parent_path = os.path.split(os.getcwd())[0];
modules_path = os.path.join(parent_path, "AObjectServer");

if (verbose == 1):
	print "verbose="+str(verbose);
	print "dryrun="+str(dryrun);
	print "target_path_DEBUG="+target_path_DEBUG;
	print "target_path_RELEASE="+target_path_RELEASE;
	print "parent_path="+parent_path;
	print "modules_path="+modules_path;

if (clean == 1):
	print "Cleaning target directory: "+os.path.join(target_path_DEBUG, "aos_root");
	makeSystemCall("del /Q /S "+os.path.join(target_path_DEBUG, "aos_root"));
	print "Cleaning target directory: "+os.path.join(target_path_RELEASE, "aos_root");
	makeSystemCall("del /Q /S "+os.path.join(target_path_RELEASE, "aos_root"));

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

# Copy extra needed stuff
syncModule2("docs","aos_root/static/");
syncModule("openssl_create_selfsigned_certificate.py");
syncModule("openssl.cnf");
