#!/usr/bin/python
import os,sys;

class Context:
  flags = {};
  def __init__(self, devenv_bin):
    self.flags['win32'] = 0;
    self.flags['win64'] = 0;
    self.flags['linux32'] = 0;
    self.flags['linux64'] = 0;
    self.flags['clean'] = 0;
    self.flags['verbose'] = 0;
    self.flags['dryrun'] = 0;
    self.BASE_DEVENV_BIN_PATH=os.path.abspath(devenv_bin);
    self.BASE_CODE_PATH=os.path.normpath(os.path.join(self.BASE_DEVENV_BIN_PATH, "..", ".."));
    self.BASE_ALIBRARY_PATH=os.path.join(self.BASE_CODE_PATH, "ALibrary");
    self.BASE_ALIBRARYIMPL_PATH=os.path.join(self.BASE_CODE_PATH, "ALibraryImpl");
    self.BASE_AOBJECTSERVER_PATH=os.path.join(self.BASE_CODE_PATH, "AObjectServer");
    self.BASE_AOSMODULES_PATH=os.path.join(self.BASE_CODE_PATH, "AOS_Modules");
    self.DEBUG_32 = os.path.join(self.BASE_CODE_PATH,"_debug");
    self.RELEASE_32 = os.path.join(self.BASE_CODE_PATH,"_release");
    self.DEBUG_64 = os.path.join(self.BASE_CODE_PATH,"_debug64a");
    self.RELEASE_64 = os.path.join(self.BASE_CODE_PATH,"_release64a");

  def __str__(self):
    ret = "";
    for item in self.flags.items():
      ret = ret + str(item[0]) + "=" + str(item[1]) + "\r\n";
    return ret;

  def __getattr__(self, name):
    if (self.flags.has_key(name)):
      return self.flags[name];
    else:
      return 0;

  def __setattr__(self, name, value):
    self.flags[name]=value;

  def hasNoPlatform(self):
    return (self.win32 == 0 and self.win64 == 0 and self.linux32 == 0 and self.linux64 == 0);


def syncPath(context, source_path, target_path):
  EXEC_RSYNC_BASE="rsync -tuC --exclude=.svn ";
  CMD = EXEC_RSYNC_BASE;
  if (context.verbose == 1):
    CMD = CMD + "-v ";
  if (context.dryrun == 1):
    CMD = CMD + "-n ";
  if (context.recursive == 1):
    CMD = CMD + "-r ";
  CMD = CMD + source_path + " " + target_path;
  if (context.verbose == 1):
    print(os.getcwd()+":"+CMD);
  os.system(CMD);
def syncPathFiles(context, source_path, filemask, target_path):
  EXEC_RSYNC_BASE="rsync -tuC --exclude=.svn --exclude=pch*.hpp ";
  CMD = EXEC_RSYNC_BASE;
  if (context.verbose == 1):
    CMD = CMD + "-v ";
  if (context.dryrun == 1):
    CMD = CMD + "-n ";
  if (context.recursive == 1):
    CMD = CMD + "-r ";
  CMD = CMD + os.path.join(source_path,filemask) + " " + target_path;
  if (context.verbose == 1):
    print(os.getcwd()+":"+CMD);
  os.system(CMD);
def syncFile(context, source_file, target_path):
  EXEC_RSYNC_BASE="rsync -tuC ";
  CMD = EXEC_RSYNC_BASE;
  if (context.verbose == 1):
    CMD = CMD + "-v ";
  if (context.dryrun == 1):
    CMD = CMD + "-n ";
  CMD = CMD + source_file + " " + target_path;
  if (context.verbose == 1):
    print(os.getcwd()+":"+CMD);
  os.system(CMD);
def syncFileToOutput(context, source_file):
  if (context.win32 == 1 or context.linux32 == 1):
    syncFile(context, source_file, context.DEBUG_32);
    syncFile(context, source_file, context.RELEASE_32);
  if (context.win64 == 1 or context.linux64 == 1):
    syncFile(context, source_file, context.DEBUG_64);
    syncFile(context, source_file, context.RELEASE_64);
