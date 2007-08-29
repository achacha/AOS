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

RSYNC_SCRIPT = os.getcwd()+"/rsync.py";
EXEC_RSYNC_BASE=RSYNC_SCRIPT+" -tuC --exclude=CVS --exclude=pch*.hpp ";
def syncPath(source_path, target_path):
	CMD = EXEC_RSYNC_BASE + source_path + " " + target_path; 
	if (verbose == 1):
		print CMD;
	os.system(CMD);

def syncPathWithDelete(source_path, target_path):
	CMD = EXEC_RSYNC_BASE + "--delete " + source_path + " " + target_path; 
	if (verbose == 1):
		print CMD;
	os.system(CMD);

def makeSystemCall_remove(filename):
	CMD = "del /q \""+filename+"\"";
	if (verbose == 1):
		print CMD;
	os.system(CMD);

def showUsage():
	print "Usage: "+sys.argv[0]+" <target path> [clean | verbose]";
	print " target path = where everything is copied to (default: ..\\)";
	print "e.g. this C:\\AOS\\";

######################################################################
########################## MAIN ######################################
######################################################################

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
	else:
		print "Unknown parameter: "+sys.argv[argc];
		showUsage();
		sys.exit(-1);
	
	argc += 1;

if (TARGET_PATH == "/?" or TARGET_PATH == "?"):
	showUsage();
	sys.exit(0);

TARGET_ALIBRARY_INCLUDE_PATH = os.path.join(TARGET_PATH, "ALibrary", "include");

base_path = os.path.join(os.getcwd());
base_path = os.path.split(base_path)[0];

# Target paths
makeSystemCall_mkdir(TARGET_PATH);

# Create or clean include path
if (not os.path.exists(TARGET_ALIBRARY_INCLUDE_PATH)):
	makeSystemCall_mkdir(TARGET_ALIBRARY_INCLUDE_PATH);
elif (clean == 1):
	makeSystemCall_remove(os.path.join(TARGET_ALIBRARY_INCLUDE_PATH, "*.*"));

# ALibrary include
print "|----------HEADERS: ALibrary/ABase-------------------------|";
if (clean == 1):
	syncPathWithDelete(os.path.join(base_path, "ALibrary", "ABase", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
else:
	syncPath(os.path.join(base_path, "ALibrary", "ABase", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_MySQL
print "|----------HEADERS: ALibraryImpl/ADatabase_MySQL-----------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_MySQL", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_SQLite
print "|----------HEADERS: ALibraryImpl/ADatabase_SQLite----------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_SQLite", "*.hpp"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_SQLite", "sqlite3.h"), TARGET_ALIBRARY_INCLUDE_PATH);

# ADatabase_ODBC
print "|----------HEADERS: ALibraryImpl/ADatabase_ODBC------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "ADatabase_ODBC", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# APythonEmbed
print "|----------HEADERS: ALibraryImpl/APythonEmbed--------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "APythonEmbed", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# AXsl
print "|----------HEADERS: ALibraryImpl/AXsl----------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AXsl", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# AZlib
print "|----------HEADERS: ALibraryImpl/AZlib---------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AZlib", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);

# ACrypto
print "|----------HEADERS: ALibraryImpl/ACrypto-------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "ACrypto", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
syncPath(os.path.join(base_path, "ALibraryImpl", "ACrypto", "openssl", "*.h*"), os.path.join(TARGET_ALIBRARY_INCLUDE_PATH, "openssl"));

# AGdLib
print "|----------HEADERS: ALibraryImpl/AGdLib--------------------|";
syncPath(os.path.join(base_path, "ALibraryImpl", "AGdLib", "*.h*"), TARGET_ALIBRARY_INCLUDE_PATH);
