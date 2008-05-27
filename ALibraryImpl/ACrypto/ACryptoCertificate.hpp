/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__ACryptoCertificate_HPP__
#define INCLUDED__ACryptoCertificate_HPP__

#include "apiACrypto.hpp"
#include "ACryptoInterface.hpp"

class ACRYPTO_API ACryptoCertificate : public ACryptoInterface
{
public:

  ACryptoCertificate();
  ACryptoCertificate(const char *, int);

  virtual ~ACryptoCertificate();

  int read_certificate();
  int read_certificate(const char *, int = FORMAT_PEM);
  int read_certificate(ACryptoInterface &, int = FORMAT_PEM);

  int read_certificate_request();
  int read_certificate_request(const char *, int = FORMAT_PEM);
  int read_certificate_request(ACryptoInterface &, int = FORMAT_PEM);

  int write_certificate();
  int write_certificate(const char *, int = FORMAT_PEM);
  int write_certificate(ACryptoInterface &, int f = FORMAT_PEM);

  int read_private_key();
  int read_private_key(const char *, int = FORMAT_PEM);
  int read_private_key(ACryptoInterface &, int = FORMAT_PEM);

  const char *get_in_filename() { return cert_file; }
  const char *get_out_filename() { return out_cert_file; }

  const char *get_certificate_request_filename() { return cert_req_file; }

  int set_in_cert_filename(const char *);
  int set_out_cert_filename(const char *);

  int set_certificate_request_filename(const char *);

  int get_key_format() { return key_format; }
  int get_in_cert_format() { return in_format; }
  int get_out_cert_format() { return out_format; }

  int set_key_format(int);
  int set_in_cert_format(int);
  int set_out_cert_format(int);

  int self_sign(int);

  void print_cert();
  void oneline();

  enum cert_format { 
    FORMAT_INVALID = 0,
    FORMAT_ASN1 = 1, FORMAT_DER = 1,
    FORMAT_TEXT = 2, 
    FORMAT_PEM = 3, 
    FORMAT_NETSCAPE = 4, FORMAT_NET = 4
  };

protected:

  X509_REQ *cert_req;
  X509 *cert, *CA_cert;

  EVP_PKEY *privkey, *CA_privkey;

  char *cert_file, *out_cert_file, *cert_req_file;
  char *key_file;

  char *CA_cert_file, *out_CA_cert_file;
  char *CA_key_file;

  int key_format, in_format, out_format;

  void _initialize_cert();
  void _clear_cert();

};

#endif //INCLUDED__ACryptoCertificate_HPP__
