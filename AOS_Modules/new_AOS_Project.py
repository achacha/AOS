#!/usr/bin/python
import os, sys, zipfile;

def getGUID():
	return (os.popen2("GuidGen")[1]).readline().strip();

if (len(sys.argv) < 3):
	print "Usage: "+os.path.split(sys.argv[0])[1]+" [Name Of the new project] [Name of the new module in project]";
	print "Example: "+os.path.split(sys.argv[0])[1]+" MyNewProject SomeNewModule";
	sys.exit(-1);

projectName = sys.argv[1];
projectNameUpperCase = projectName.upper();
moduleName = sys.argv[2];

if (os.path.exists("AOS_"+projectName)):
	print "Directory AOS_"+projectName+" already exists, please remove it.";
	sys.exit(-1);

print "Creating new project: "+projectName;

AOS_PROJECT_ZIPFILE = "AOS_SAMPLEPROJECT.zip";

aoszip = zipfile.ZipFile(AOS_PROJECT_ZIPFILE);
zipfilelist = aoszip.namelist();

for name in zipfilelist:
	newname = name.replace("SAMPLEPROJECT", projectName);
	newname = newname.replace("SAMPLEMODULE", moduleName);
	
	if (newname[-1] == '/'):
		# Only new directory
		if (not os.path.exists(newname)):
			os.makedirs(newname);
	else:
		# File, process contents and save
		filedata = aoszip.read(name);

		while(filedata.find("{SAMPLEPROJECTGUID}") !=	-1):
			newguid = getGUID();
			filedata = filedata.replace("{SAMPLEPROJECTGUID}",	newguid, 1);

		filedata = filedata.replace("SAMPLEPROJECTUPPERCASE", projectNameUpperCase);
		filedata = filedata.replace("SAMPLEPROJECT", projectName);
		filedata = filedata.replace("SAMPLEMODULE",	moduleName);

		newfile = open(newname, "wb");
		newfile.write(filedata);
		newfile.close();
	
	print "Created: "+newname;
	
