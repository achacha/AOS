#!/usr/bin/python
import os,sys;

RSYNC_PARAMS="-tupEgo --progress --exclude=.svn";

class Context:
  flags = {};
  def __init__(self, devenv_bin):
    if (not os.path.exists(os.path.join(devenv_bin, "acommon.py"))):
      raise RuntimeError("Invalid dev bin path: "+devenv_bin);
    
    self.flags['win32'] = 0;
    self.flags['win64'] = 0;
    self.flags['linux32'] = 0;
    self.flags['linux64'] = 0;
    self.flags['clean'] = 0;
    self.flags['verbose'] = 0;
    self.flags['dryrun'] = 0;
    self.flags['binary'] = 0;
    
    self.BASE_DEVENV_BIN_PATH=os.path.abspath(devenv_bin);
    self.BASE_DEVENV_PATH=os.path.normpath(os.path.join(self.BASE_DEVENV_BIN_PATH, ".."));
    self.BASE_CODE_PATH=os.path.normpath(os.path.join(self.BASE_DEVENV_PATH, ".."));
    self.setTargetPath(self.BASE_CODE_PATH);
    self.BASE_ALIBRARY_PATH=os.path.join(self.BASE_CODE_PATH, "ALibrary");
    self.BASE_ALIBRARYIMPL_PATH=os.path.join(self.BASE_CODE_PATH, "ALibraryImpl");
    self.BASE_AOBJECTSERVER_PATH=os.path.join(self.BASE_CODE_PATH, "AObjectServer");
    self.BASE_AOSMODULES_PATH=os.path.join(self.BASE_CODE_PATH, "AOS_Modules");
    
  def setTargetPath(self, target_path):
    self.TARGET_PATH = os.path.normpath(target_path);
    self.DEBUG_32 = os.path.join(self.BASE_CODE_PATH,"_debug");
    self.DEBUG_64 = os.path.join(self.BASE_CODE_PATH,"_debug64a");
    self.RELEASE_32 = os.path.join(self.BASE_CODE_PATH,"_release");
    self.RELEASE_64 = os.path.join(self.BASE_CODE_PATH,"_release64a");
    self.TARGET_DEBUG_32 = os.path.join(self.TARGET_PATH,"_debug");
    self.TARGET_DEBUG_64 = os.path.join(self.TARGET_PATH,"_debug64a");
    if (self.binary == 0):
      self.TARGET_RELEASE_32 = os.path.join(self.TARGET_PATH,"_release");
      self.TARGET_RELEASE_64 = os.path.join(self.TARGET_PATH,"_release64a");
    else:
      self.TARGET_RELEASE_32 = os.path.join(self.TARGET_PATH,"bin");
      self.TARGET_RELEASE_64 = os.path.join(self.TARGET_PATH,"bin64");

  def processArgv(self, argv):
    self.THIS_PATH = argv[0];
    argc = 1;
    target_path = "";
    while (len(sys.argv) > argc):
      if (sys.argv[argc] == "-clean"):
        self.flags['clean'] = 1;
      elif (sys.argv[argc] == "-v" or sys.argv[argc] == "-verbose"):
        self.flags['verbose'] = 1;
      elif (sys.argv[argc] == "-dryrun"):
        self.flags['dryrun'] = 1;
      elif (sys.argv[argc] == "-binary"):
        self.flags['binary'] = 1;
      elif (sys.argv[argc] == "-win32"):
        self.flags['win32'] = 1;
      elif (sys.argv[argc] == "-win64"):
        self.flags['win64'] = 1;
      elif (sys.argv[argc] == "-linux32"):
        self.flags['linux32'] = 1;
      elif (sys.argv[argc] == "-linux64"):
        self.flags['linux64'] = 1;
      elif (sys.argv[argc] == "-p"):
        target_path = sys.argv[argc+1];
        argc += 1;
      elif (sys.argv[argc] == "-help"):
        showUsage();
        return -1;
      else:
        print "Unknown parameter: "+sys.argv[argc];
        return -1;
      argc += 1;
    
    if (target_path != ""):
      self.setTargetPath(target_path);
    if (self.verbose == 1):
      print(self.__str__());

  def getArgv(self):
    params = "";
    if (self.clean != 0):
      params = params + " -clean";
    if (self.verbose != 0):
      params = params + " -v";
    if (self.dryrun != 0):
      params = params + " -dryrun";
    if (self.binary != 0):
      params = params + " -binary";
    if (self.win32 != 0):
      params = params + " -win32";
    if (self.win64 != 0):
      params = params + " -win64";
    if (self.linux32 != 0):
      params = params + " -linux32";
    if (self.linux64 != 0):
      params = params + " -linux64";
    if (len(self.TARGET_PATH) > 0):
      params = params + " -p "+self.TARGET_PATH;
    return params;

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
    
  def targetDoesNotExist(self):
    if (self.binary == 1):
      return (self.win32 == 1 and not os.path.exists(self.TARGET_RELEASE_32)) or (self.win64 == 1 and not os.path.exists(self.TARGET_RELEASE_64));
    else:
      return (self.win32 == 1 and not os.path.exists(self.TARGET_DEBUG_32)) or (self.win64 == 1 and not os.path.exists(self.TARGET_DEBUG_64));

  def executeScript(self, scriptname):
    curdir = os.getcwd();
    os.chdir(self.BASE_DEVENV_BIN_PATH);
    CMD = "python " + scriptname + self.getArgv();
    if (self.verbose == 1):
      print("callScript: "+CMD);
    os.system(CMD);
    os.chdir(curdir);
  
  def makeSystemCall(self, CMD):
    if (self.verbose == 1):
      print("makeSystemCall: "+os.getcwd()+": "+CMD);
    os.system(CMD);

  def syncPath(self, source_path, target_path):
    EXEC_RSYNC_BASE="rsync -tup --exclude=.svn ";
    CMD = EXEC_RSYNC_BASE;
    if (self.verbose == 1):
      CMD = CMD + "-v ";
    if (self.dryrun == 1):
      CMD = CMD + "-n ";
    if (self.recursive == 1):
      CMD = CMD + "-r ";
    CMD = CMD + source_path + " " + target_path;
    if (self.verbose == 1):
      print(os.getcwd()+":"+CMD);
    os.system(CMD);

  def syncPathRecursive(self, source_path, target_path):
    EXEC_RSYNC_BASE="rsync -tup --exclude=.svn -r ";
    CMD = EXEC_RSYNC_BASE;
    if (self.verbose == 1):
      CMD = CMD + "-v ";
    if (self.dryrun == 1):
      CMD = CMD + "-n ";
    CMD = CMD + source_path + " " + target_path;
    if (self.verbose == 1):
      print(os.getcwd()+":"+CMD);
    os.system(CMD);

  def syncPathFiles(self, source_path, filemask, target_path):
    EXEC_RSYNC_BASE="rsync "+RSYNC_PARAMS+" --exclude=pch*.hpp ";
    CMD = EXEC_RSYNC_BASE;
    if (self.verbose == 1):
      CMD = CMD + "-v ";
    if (self.dryrun == 1):
      CMD = CMD + "-n ";
    if (self.recursive == 1):
      CMD = CMD + "-r ";
    CMD = CMD + os.path.join(source_path,filemask) + " " + target_path;
    if (self.verbose == 1):
      print(os.getcwd()+":"+CMD);
    os.system(CMD);

  def syncPathFilesWithPCH(self, source_path, filemask, target_path):
    EXEC_RSYNC_BASE="rsync "+RSYNC_PARAMS+" ";
    CMD = EXEC_RSYNC_BASE;
    if (self.verbose == 1):
      CMD = CMD + "-v ";
    if (self.dryrun == 1):
      CMD = CMD + "-n ";
    if (self.recursive == 1):
      CMD = CMD + "-r ";
    CMD = CMD + os.path.join(source_path,filemask) + " " + target_path;
    if (self.verbose == 1):
      print(os.getcwd()+":"+CMD);
    os.system(CMD);

  def syncFile(self, source_file, target_path):
    EXEC_RSYNC_BASE="rsync "+RSYNC_PARAMS+" ";
    CMD = EXEC_RSYNC_BASE;
    if (self.verbose == 1):
      CMD = CMD + "-v ";
    if (self.dryrun == 1):
      CMD = CMD + "-n ";
    CMD = CMD + source_file + " " + target_path;
    if (self.verbose == 1):
      print(os.getcwd()+":"+CMD);
    os.system(CMD);

  def syncFileToOutput(self, source_file):
    if (self.win32 == 1 or self.linux32 == 1):
      if (self.binary == 0):
        self.syncFile(source_file, self.TARGET_DEBUG_32);
      self.syncFile(source_file, self.TARGET_RELEASE_32);
    if (self.win64 == 1 or self.linux64 == 1):
      if (self.binary == 0):
        self.syncFile(source_file, self.TARGET_DEBUG_64);
      self.syncFile(source_file, self.TARGET_RELEASE_64);

  def syncExecutable(self, name):
    if (self.win32 == 1):
      if (self.binary == 0):
        self.syncFile(os.path.join(self.DEBUG_32, name+".exe"), self.TARGET_DEBUG_32);
        self.syncFile(os.path.join(self.DEBUG_32, name+".pdb"), self.TARGET_DEBUG_32);
      self.syncFile(os.path.join(self.RELEASE_32, name+".exe"), self.TARGET_RELEASE_32);
      self.syncFile(os.path.join(self.RELEASE_32, name+".map"), self.TARGET_RELEASE_32);
    
    if (self.win64 == 1):
      if (self.binary == 0):
        self.syncFile(os.path.join(self.DEBUG_64, name+".exe"), self.TARGET_DEBUG_64);
        self.syncFile(os.path.join(self.DEBUG_64, name+".pdb"), self.TARGET_DEBUG_64);
      self.syncFile(os.path.join(self.RELEASE_64, name+".exe"), self.TARGET_RELEASE_64);
      self.syncFile(os.path.join(self.RELEASE_64, name+".map"), self.TARGET_RELEASE_64);

    if (self.linux32 == 1):
      raise NotImplementedError;
      
    if (self.linux64 == 1):
      raise NotImplementedError;

  def syncLibrary(self, name):
    if (self.win32 == 1):
      if (self.binary == 0):
        self.syncFile(os.path.join(self.DEBUG_32, name+".dll"), self.TARGET_DEBUG_32);
        self.syncFile(os.path.join(self.DEBUG_32, name+".lib"), self.TARGET_DEBUG_32);
        self.syncFile(os.path.join(self.DEBUG_32, name+".pdb"), self.TARGET_DEBUG_32);
        self.syncFile(os.path.join(self.RELEASE_32, name+".lib"), self.TARGET_RELEASE_32);
      self.syncFile(os.path.join(self.RELEASE_32, name+".dll"), self.TARGET_RELEASE_32);
      self.syncFile(os.path.join(self.RELEASE_32, name+".map"), self.TARGET_RELEASE_32);
    if (self.win64 == 1):
      if (self.binary == 0):
        self.syncFile(os.path.join(self.DEBUG_64, name+".dll"), self.TARGET_DEBUG_64);
        self.syncFile(os.path.join(self.DEBUG_64, name+".lib"), self.TARGET_DEBUG_64);
        self.syncFile(os.path.join(self.DEBUG_64, name+".pdb"), self.TARGET_DEBUG_64);
        self.syncFile(os.path.join(self.RELEASE_64, name+".lib"), self.TARGET_RELEASE_64);
      self.syncFile(os.path.join(self.RELEASE_64, name+".dll"), self.TARGET_RELEASE_64);
      self.syncFile(os.path.join(self.RELEASE_64, name+".map"), self.TARGET_RELEASE_64);
    if (self.linux32 == 1):
      raise NotImplementedError;
    if (self.linux64 == 1):
      raise NotImplementedError;

  def syncAosRoot(self, basepath):
    print("|----------------------------Content: "+basepath+" -> "+self.TARGET_PATH);
    self.syncPathRecursive(os.path.join(basepath, "aos_root"), self.TARGET_PATH);

  def showUsage(self):
    # Basic usage
    print(self.__str__());
    print("");
    print("Usage: "+sys.argv[0]+" [options]");
    print(" -win32             - Create windows 32-bit distro");
    print(" -win64             - Create windows 64-bit distro");
    print(" -linux32           - Create linux 32-bit distro");
    print(" -linux64           - Create linux 64-bit distro");
    print(" -p <target path>   - Will create 'ALibrary/include' directory and gather all pertinent headers");
    print(" -binary            - Sync release binaries only");
    print(" -dryrun            - Sync as dry run");
    print(" -clean             - Clean first");
    print(" -v                 - Verbose mode");
