#!/usr/bin/python
import os,sys;

class Context:
  flags = {};
  def __init__(self, devenv_bin):
    self.flags['win32'] = 0;
    self.flags['win64'] = 0;
    self.flags['linux'] = 0;
    self.flags['clean'] = 0;
    self.flags['verbose'] = 0;
    self.flags['dryrun'] = 0;
    self.BASE_DEVENV_BIN_PATH=os.path.abspath(devenv_bin);
    self.BASE_CODE_PATH=os.path.normpath(os.path.join(self.BASE_DEVENV_BIN_PATH, "..", ".."));
    self.BASE_ALIBRARY_PATH=os.path.join(self.BASE_CODE_PATH, "ALibrary");
    self.BASE_ALIBRARYIMPL_PATH=os.path.join(self.BASE_CODE_PATH, "ALibraryImpl");
    self.BASE_AOBJECTSERVER_PATH=os.path.join(self.BASE_CODE_PATH, "AObjectServer");
    self.BASE_AOSMODULES_PATH=os.path.join(self.BASE_CODE_PATH, "AOS_Modules");

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


def syncPath(context, source_path, filemask, target_path):
  EXEC_RSYNC_BASE="rsync -tuC --exclude=.svn --exclude=pch*.hpp ";
  CMD = EXEC_RSYNC_BASE;
  if (context.verbose == 1):
    CMD = CMD + "-v ";
  if (context.dryrun == 1):
    CMD = CMD + "-n ";
  CMD = CMD + os.path.join(source_path,filemask) + " " + target_path;
  if (context.verbose == 1):
    print(os.getcwd()+":"+CMD);
  os.system(CMD);
