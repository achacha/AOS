import sys, os;

def process(file):
	lineno = 0;
	if (verbose != 0):
		print "  Processing file: "+file;
	f = open(file,"r");
	for line in f.readlines():
		lineno += 1;
		i = line.find("ASW(");
		if (i != -1):
			q0 = line.find("\"", i);
			if (q0 != -1):
				q1 = line.find("\"", q0+1);
				if (q1 != -1):
					p1 = line.find(")", q1+1);
					if (p1 != -1):
						c = line.find(",", q1+1);
						if (c != -1 and c < p1):
							rlen = int(line[c+1:p1]);
							llen = len(line[q0+1:q1]) - line[q0+1:q1].count('\\');
							if (llen != rlen):
								print "!!!ERROR!!!: ASW in "+file+":"+str(lineno)+" Mismatched lenths: ASW("+line[q0:p1]+") = ASW("+str(llen)+","+str(rlen)+")  SHOULD BE "+str(llen);
							if (verbose != 0):
								print " ASW "+file+":"+str(lineno)+" OK: ASW("+line[q0:p1]+") = ASW("+str(llen)+","+str(rlen)+")";
						else:
							print " !ASW "+file+":"+str(lineno)+" Can't find comma before ): "+line[q0:p1];
					else:
						print " !ASW "+file+":"+str(lineno)+" Can't find ): "+line[:-2];
				else:
					print " !ASW "+file+":"+str(lineno)+" Can't find ending double quote: "+line[:-2]; 
			else:
				print " !ASW "+file+":"+str(lineno)+" Can't find opening double quote: "+line[:-2];
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
	print "-------Checking for proper ASW length: "+path;
	files = os.listdir(path);
	for file in files:
		if (file[-4:] == ".cpp"):
			process(os.path.join(path,file));
else:
	print "Path does not exist: "+path;
