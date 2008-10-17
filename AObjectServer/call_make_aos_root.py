#!/usr/bin/python
import os,sys;

BASE_AOBJECTSERVER_PATH=os.path.dirname(os.path.abspath(sys.argv[0]));
BASE_ENV_PATH=os.path.normpath(os.path.join(BASE_AOBJECTSERVER_PATH,"..","_devtools","bin"));

params="";
for param in sys.argv[1:]:
  params = params + param + " ";

os.chdir(BASE_ENV_PATH);
os.system(os.path.join(BASE_ENV_PATH, "make_aos_root.py "+params));
