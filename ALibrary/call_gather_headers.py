#!/usr/bin/python
import os,sys;

BASE_ALIBRARY_PATH=os.path.dirname(os.path.abspath(sys.argv[0]));
#print("BASE_ALIBRARY_PATH="+BASE_ALIBRARY_PATH);
BASE_ENV_PATH=os.path.normpath(os.path.join(BASE_ALIBRARY_PATH,"..","_devtools","bin"));

params="";
for param in sys.argv[1:]:
  params = params + param + " ";

os.chdir(BASE_ENV_PATH);
print(os.getcwd());
os.system("./gather_headers.py "+params);
