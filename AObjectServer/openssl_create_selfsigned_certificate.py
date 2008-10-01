#!/bin/python

import os,sys;

def makeSystemCall(param):
	print param;
	return os.system(param);

print "(Optional) Edit the openssl.config file and update whatever fields you need"

if (not os.path.exists("openssl.exe")):
	print "openssl.exe missing from current directory (should be executed in output directory)";
	sys.exit(-1);
	
if (not os.path.exists("openssl.config")):
	print "openssl.config missing from current directory (should be executed in output directory)";
	sys.exit(-1);

OUTPUT_DIR="../aos_root/certs/";

ret = makeSystemCall("openssl genrsa -out ca_pkey.pem 2048");
if (0 != ret):
	print "Failed to create CA private key";
	sys.exit(-1);

ret = makeSystemCall("openssl req -config openssl.config -new -x509 -key ca_pkey.pem -out aos_cert.pem");
if (0 != ret):
	print "Failed to self-sign the certificate";
	sys.exit(-1);

# 1. Create private key
ret = makeSystemCall("openssl genrsa -out "+OUTPUT_DIR+"aos_pkey.pem 2048");
if (0 != ret):
	print "Failed to create private key";
	sys.exit(-1);

# 2. Self sign a certificate
ret = makeSystemCall("openssl req -config openssl.config -new -x509 -key "+OUTPUT_DIR+"aos_pkey.pem -out "+OUTPUT_DIR+"aos_cert.pem");
if (0 != ret):
	print "Failed to self-sign the certificate";
	sys.exit(-1);

# 2. OR Create cerificate request using:  openssl req -new -key aos_pkey.pem -out aos_cert.csr

print "\nCreated Private key       : "+OUTPUT_DIR+"aos_pkey.pem";
print "Created Server certificate: "+OUTPUT_DIR+"aos_cert.pem";
