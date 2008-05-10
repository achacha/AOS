import sys, os, re;

def handleClass(clazz, includes):
  if (clazz.startswith("pch")):
    if (verbose != 0):
      print "    Ignoring PCH header: "+clazz;
    return;
  
  if (clazz == "ansiHelpers"):
    if (verbose != 0):
      print "    Ignoring ansiHelpers";
    return;

  if (clazz == "ASystemException"):
    if (verbose != 0):
      print "    Remapping ASystemException to AException";
    clazz = "AException";
  
  if (clazz.startswith("template")):
    if (verbose != 0):
      print "    Converting template* to A*";
    clazz = "A"+clazz[8:];

  if (verbose != 0):
    print "    Found include class: "+clazz;

  includes[clazz] = 0;
  return;

def process(file):
  includes = {};
  lineno = 0;
  if (verbose != 0):
    print "  Processing file: "+file;
  f = open(file,"r");
  for line in f.readlines():
    lineno += 1;
    m = re.search(".*\#include\ [\"<](.+).hpp[\">]", line);
    if (m != None):
      for clazz in m.groups(0):
        handleClass(clazz, includes);
    elif (len(includes) > 0):
      #Check if one of the includes is in the current line
      if (verbose != 0):
        print line;
      for clazz in includes.keys():
        if (line.find(clazz) >= 0):
          includes[clazz] = includes[clazz] + 1;
          if (verbose != 0):
            print "  Found "+clazz;
        elif (verbose != 0):
          print "  Not found "+clazz;
    elif (verbose != 0):
      print "  Doing nothing for line (no includes yet): "+line;

  if (verbose != 0):
    print(str(includes));
  for item in includes.items():
    if (item[1] == 0):
      print " !"+file+": Possibly unused include "+item[0]+".hpp";
  f.close();
  

verbose = 0;
path = ".";
argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "verbose"):
    verbose = 1;
  else:
    path = sys.argv[argc];
  
  argc += 1;

if (verbose != 0):
  print "path    = "+path;
  print "verbose = "+str(verbose);

if (os.path.exists(path)):
  print "-------Checking for possible unused headers: "+path;
  files = os.listdir(path);
  for file in files:
    if (file[-4:] == ".cpp"):
      process(os.path.join(path,file));
else:
  print "Path does not exist: "+path;
