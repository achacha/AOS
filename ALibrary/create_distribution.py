#!/bin/python
import os,sys;
 
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

EXEC_RSYNC_BASE=".\\rsync.py -tuC --exclude=CVS ";
def syncPath(source_path, target_path):
	CMD = EXEC_RSYNC_BASE + source_path + " " + target_path; 
	if (verbose == 1):
		print CMD;
	os.system(CMD);

EXEC_RSYNC_BASE_WITH_DELETE=".\\rsync.py -tuC --exclude=CVS --delete ";
def syncPathWithDelete(source_path, target_path):
	CMD = EXEC_RSYNC_BASE_WITH_DELETE + source_path + " " + target_path; 
	if (verbose == 1):
		print CMD;
	os.system(CMD);

def touch(filename):
	if (not os.path.exists(filename)):
		f = open(filename, "w");
		print >>f,filename;
		f.close();
	

def showUsage():
	print "Usage: "+sys.argv[0]+" <target path> <debug output path> <release output path> [clean | verbose]";
	print " target path = where everything is copied to";
	print " debug output path = location of the debug build";
	print " release output path = location of the release build";
	print "e.g. this C:\\AOS\\";

def gatherHeaders():
	CMD = "gather_headers_only.py " + TARGET_PATH;
	if (verbose == 1):
		CMD += " verbose";
	if (clean == 1):
		CMD += " clean";
	makeSystemCall(CMD);	

######################################################################
########################## MAIN ######################################
######################################################################

DEBUG_INPUT_PATH = "..\\_debug";
RELEASE_INPUT_PATH = "..\\_release";
TARGET_PATH = "..\\";

verbose = 0;
clean = 0;
argc = 1;
while (len(sys.argv) > argc):
	if (sys.argv[argc] == "clean"):
		clean = 1;
	elif (sys.argv[argc] == "verbose"):
		verbose = 1;
	elif (argc == 1):
		TARGET_PATH = sys.argv[1];
	elif (argc == 2):
		DEBUG_INPUT_PATH = sys.argv[2];
	elif (argc == 3):
		RELEASE_INPUT_PATH = sys.argv[3];
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

if (TARGET_PATH == "/?" or TARGET_PATH == "?"):
	showUsage();
	sys.exit(0);

if (not os.path.exists(DEBUG_INPUT_PATH)):
	os.makedirs(DEBUG_INPUT_PATH);
	if (not os.path.exists(DEBUG_INPUT_PATH)):
		print "Unable to create '"+DEBUG_INPUT_PATH+"', may need manual intervention.";
	sys.exit(-1);

if (not os.path.exists(RELEASE_INPUT_PATH)):
	os.makedirs(RELEASE_INPUT_PATH);
	if (not os.path.exists(RELEASE_INPUT_PATH)):
		print "Unable to create '"+RELEASE_INPUT_PATH+"', may need manual intervention.";
	sys.exit(-1);
	
base_path = os.path.join(os.getcwd());
base_path = os.path.split(base_path)[0];

# Target paths
makeSystemCall_mkdir(TARGET_PATH);

# Path for binaries
target_bin_path_DEBUG = os.path.join(TARGET_PATH, "_debug");
makeSystemCall_mkdir(target_bin_path_DEBUG);
target_bin_path_RELEASE = os.path.join(TARGET_PATH, "_release");
makeSystemCall_mkdir(target_bin_path_RELEASE);

if (verbose == 1):
	print "Input DEBUG path    : "+DEBUG_INPUT_PATH;
	print "Input RELEASE path  : "+RELEASE_INPUT_PATH;
	print "Output target path  : "+TARGET_PATH;
	print "Output DEBUG path   : "+target_bin_path_DEBUG;
	print "Output RELEASE path : "+target_bin_path_RELEASE;

# Base libraries
print "|---------EXTERNAL: ALibrary-----------|";
if (clean == 1):
	syncPathWithDelete(os.path.join(base_path, "ALibrary", "lib", "release", "*.*"), target_bin_path_RELEASE);
	syncPathWithDelete(os.path.join(base_path, "ALibrary", "lib", "debug", "*.*"), target_bin_path_DEBUG);
else:
	syncPath(os.path.join(base_path, "ALibrary", "lib", "release", "*.*"), target_bin_path_RELEASE);
	syncPath(os.path.join(base_path, "ALibrary", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# ACrypto
print "|---------EXTERNAL: ALibraryImpl/ACrypto-----------|";
if (clean == 1):
	syncPathWithDelete(os.path.join(base_path, "ALibraryImpl", "ACrypto", "lib", "release", "*.*"), target_bin_path_RELEASE);
	syncPathWithDelete(os.path.join(base_path, "ALibraryImpl", "ACrypto", "lib", "debug", "*.*"), target_bin_path_DEBUG);
else:
	syncPath(os.path.join(base_path, "ALibraryImpl", "ACrypto", "lib", "release", "*.*"), target_bin_path_RELEASE);
	syncPath(os.path.join(base_path, "ALibraryImpl", "ACrypto", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# ADatabase_MySQL
print "|---------EXTERNAL: ALibraryImpl/ADatabase_MySQL-----------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_MySQL", "lib", "opt", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_MySQL", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# APythonEmbed
print "|---------EXTERNAL: ALibraryImpl/APythonEmbed--------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "APythonEmbed", "lib", "release", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(base_path, "ALibraryImpl", "APythonEmbed", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# AZlib
print "|---------EXTERNAL: ALibraryImpl/AZlib---------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AZlib", "lib", "release", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(base_path, "ALibraryImpl", "AZlib", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# AXsl
print "|---------EXTERNAL: ALibraryImpl/AXsl----------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AXsl", "lib", "release", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(base_path, "ALibraryImpl", "AXsl", "lib", "debug", "*.*"), target_bin_path_DEBUG);

# AXsl
print "|---------EXTERNAL: ALibraryImpl/AXsl----------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AGdLib", "lib", "*.*"), target_bin_path_RELEASE);
syncPath(os.path.join(base_path, "ALibraryImpl", "AGdLib", "lib", "*.*"), target_bin_path_DEBUG);

# Gather headers
gatherHeaders();

# Copy build results if creating external distribution
if (os.path.join(target_bin_path_DEBUG, "foo") != os.path.join(DEBUG_INPUT_PATH, "foo")):
	# ALibrary DEBUG targets
	print "|-----DEBUG OUTPUT: ALibrary-------------------------------|";
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ABase.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ABase.lib"), target_bin_path_DEBUG);

	# ALibraryImpl DEBUG targets
	print "|-----DEBUG OUTPUT: ALibraryImpl---------------------------|";
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ACrypto.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ACrypto.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_ODBC.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_ODBC.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_MySQL.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_MySQL.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_SQLite.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ADatabase_SQLite.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ALuaEmbed.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "ALuaEmbed.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "APythonEmbed.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "APythonEmbed.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AXsl.lib"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AXsl.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AZlib.dll"), target_bin_path_DEBUG);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AZlib.lib"), target_bin_path_DEBUG);

	# ALibrary RELEASE targets
	print "|---RELEASE OUTPUT: ALibrary-------------------------------|";
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ABase.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ABase.lib"), target_bin_path_RELEASE);

	# ALibraryImpl RELEASE targets
	print "|---RELEASE OUTPUT: ALibraryImpl---------------------------|";
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ACrypto.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ACrypto.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_ODBC.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_ODBC.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_MySQL.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_MySQL.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_SQLite.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ADatabase_SQLite.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(DEBUG_INPUT_PATH, "AGdLib.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ALuaEmbed.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "ALuaEmbed.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "APythonEmbed.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "APythonEmbed.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "AXsl.lib"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "AXsl.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "AZlib.dll"), target_bin_path_RELEASE);
	syncPath(os.path.join(RELEASE_INPUT_PATH, "AZlib.lib"), target_bin_path_RELEASE);

print "|-----------TOUCH MARKERS----------------------------------|";
touch(os.path.join(target_bin_path_DEBUG, "___DEBUG_OUTPUT___"));
touch(os.path.join(target_bin_path_RELEASE, "___RELEASE_OUTPUT___"));
