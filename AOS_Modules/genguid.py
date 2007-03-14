import sys,os;

def getGUID():
	iostream = os.popen2("GuidGen");
	result = iostream[1];
	return result.readline().rstrip();

print getGUID();
print getGUID();
print getGUID();
