#!/usr/bin/python
import os,sys;
from acommon import *;

def showUsage():
  print("Usage: "+sys.argv[0]+" [options]");
  print(" -win32             - Create windows 32-bit distro");
  print(" -win64             - Create windows 64-bit distro");
  print(" -linux32           - Create linux 32-bit distro");
  print(" -linux64           - Create linux 64-bit distro");
  print(" -p <target path>   - Will create 'ALibrary/include' directory and gather all pertinent headers");
  print(" -dryrun            - Sync as dry run");
  print(" -clean             - Clean first");
  print(" -v                 - Verbose mode");

######################################################################
########################### MAIN #######################################
######################################################################

context = Context(os.getcwd());
context.TARGET_BASE_PATH = context.BASE_CODE_PATH;

argc = 1;
while (len(sys.argv) > argc):
  if (sys.argv[argc] == "-clean"):
    context.clean = 1;
  elif (sys.argv[argc] == "-v"):
    context.verbose = 1;
  elif (sys.argv[argc] == "-dryrun"):
    context.dryrun = 1;
  elif (sys.argv[argc] == "-win32"):
    context.win32 = 1;
  elif (sys.argv[argc] == "-win64"):
    context.win64 = 1;
  elif (sys.argv[argc] == "-linux32"):
    context.linux32 = 1;
  elif (sys.argv[argc] == "-linux64"):
    context.linux64 = 1;
  elif (sys.argv[argc] == "-p"):
    context.TARGET_BASE_PATH = sys.argv[argc+1];
    argc += 1;
  else:
    print("Unknown parameter: "+sys.argv[argc]);
    showUsage();
    sys.exit(-1);

  argc += 1;

if (context.hasNoPlatform()):
  print("Please specify -win32 and/or -win64 and/or -linux32 and/or linux64");
  showUsage();
  sys.exit(-1);

context.TARGET_ALIBRARY_INCLUDE_PATH_32 = os.path.join(context.TARGET_BASE_PATH, "include");
context.TARGET_ALIBRARY_INCLUDE_PATH_64 = os.path.join(context.TARGET_BASE_PATH, "include64a");

if (context.verbose == 1):
  print(str(context));

# Target paths
if (context.TARGET_BASE_PATH != "" and not os.path.exists(context.TARGET_BASE_PATH)):
  os.makedirs(context.TARGET_BASE_PATH);

# Pre-clean if needed
if (context.clean == 1):
  if (context.win32 == 1 or context.linux32 == 1):
    print("Removing: "+context.TARGET_ALIBRARY_INCLUDE_PATH_32);
    os.system("rm -Ir "+context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  if (context.win64 == 1 or context.linux64 == 1):
    print("Removing: "+context.TARGET_ALIBRARY_INCLUDE_PATH_64);
    os.system("rm -Ir "+context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# Create directories
if (context.win32 == 1 or context.linux32 == 1):
  if (not os.path.exists(context.TARGET_ALIBRARY_INCLUDE_PATH_32)):
    os.makedirs(context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  if (not os.path.exists(context.TARGET_ALIBRARY_INCLUDE_PATH_64)):
    os.makedirs(context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ALibrary include
print("|----------HEADERS: ALibrary/ABase-------------------------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARY_PATH, "ABase"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARY_PATH, "ABase"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ADatabase_MySQL
print("|----------HEADERS: ALibraryImpl/ADatabase_MySQL-----------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "include"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_MySQL", "include64a"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ADatabase_SQLite
print("|----------HEADERS: ALibraryImpl/ADatabase_SQLite----------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_SQLite"), "*.hpp", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_SQLite"), "sqlite3.h", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_SQLite"), "*.hpp", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_SQLite"), "sqlite3.h", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ADatabase_ODBC
print("|----------HEADERS: ALibraryImpl/ADatabase_ODBC------------|");
if (context.win32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_ODBC"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ADatabase_ODBC"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ALuaEmbed
print("|----------HEADERS: ALibraryImpl/ALuaEmbed-----------------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ALuaEmbed"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ALuaEmbed", "lua-5.1.3", "src"), "*.h", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ALuaEmbed"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ALuaEmbed", "lua-5.1.3", "src"), "*.h", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# AZlib
print("|----------HEADERS: ALibraryImpl/AZlib---------------------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AZlib"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);

# ACrypto
print("|----------HEADERS: ALibraryImpl/ACrypto-------------------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "openssl"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_32, "openssl"));
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "ACrypto", "openssl"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_64, "openssl"));

# AGdLib
print("|----------HEADERS: ALibraryImpl/AGdLib--------------------|");
if (context.win32 == 1 or context.linux32 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libgd"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libpng"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libjpeg", "include"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_32);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_32, "freetype"));
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype", "config"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_32, "freetype", "config"));
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype", "internal"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_32, "freetype", "internal"));
if (context.win64 == 1 or context.linux64 == 1):
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libgd"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libpng"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "libjpeg", "include64a"), "*.h*",context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include"), "*.h*", context.TARGET_ALIBRARY_INCLUDE_PATH_64);
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_64, "freetype"));
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype", "config"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_64, "freetype", "config"));
  syncPathFiles(context, os.path.join(context.BASE_ALIBRARYIMPL_PATH, "AGdLib", "freetype", "include", "freetype", "internal"), "*.h*", os.path.join(context.TARGET_ALIBRARY_INCLUDE_PATH_64, "freetype", "internal"));
