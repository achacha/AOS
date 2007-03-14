#include "pchACrypto.hpp"
#include "ACryptoCertificate.hpp"

#define CERTIFICATE_HEADER	"certificate"

#define VALID_CERT_FORMAT(x)	((x == FORMAT_PEM) || \
				 (x == FORMAT_DER) || \
				 (x == FORMAT_NET))


#define DAYS_TO_SECONDS(x)	(x * 86400)	// 60*60*24

#define	X509_OBJ_ID		"2.99999.3"
#define X509_OBJ_SN		"SET.ex3"
#define X509_OBJ_LN		"SET x509V3 extension 3"

ACryptoCertificate::ACryptoCertificate()
{
  _initialize_cert();
}

ACryptoCertificate::ACryptoCertificate(const char *filename, int mode)
{
  if((!filename) || (!*filename) || !VALID_CERT_FORMAT(mode)) 
    return;

  _initialize_cert();

  in_format = mode;

  cert_file = (char *)filename;

  read_certificate();
}

ACryptoCertificate::~ACryptoCertificate()
{
  _clear_cert();
}

void ACryptoCertificate::_initialize_cert()
{
  cert_req = NULL;

  cert = NULL;
  CA_cert = NULL;
  
  privkey = NULL;
  CA_privkey = NULL;

  cert_file = NULL;
  key_file = NULL;

  CA_cert_file = NULL;
  CA_key_file = NULL;

  key_format = 0;
  in_format = 0;
  out_format = 0;
}  

void ACryptoCertificate::_clear_cert()
{
  if(cert_req) {
    X509_REQ_free(cert_req);
    
    cert_req = NULL;
  }

  if(cert) {
    X509_free(cert);
    
    cert = NULL;
  }

  if(CA_cert) {
    X509_free(CA_cert);
    
    CA_cert = NULL;
  }

  if(privkey) {
    EVP_PKEY_free(privkey);

    privkey = NULL;
  }

  if(CA_privkey) {
    EVP_PKEY_free(CA_privkey);

    CA_privkey = NULL;
  }

  if(CA_cert_file) {
    free(CA_cert_file);

    CA_cert_file = NULL;
  }

  if(CA_key_file) {
    free(CA_key_file);

    CA_key_file = NULL;
  }

  if(cert_file) {
    free(cert_file);
    
    cert_file = NULL;
  }

  if(out_cert_file) {
    free(out_cert_file);

    out_cert_file = NULL;
  }

  if(cert_req_file) {
    free(cert_req_file);
    
    cert_req_file = NULL;
  }

  key_format = 0;
  in_format = 0;
  out_format = 0;
}
    

int ACryptoCertificate::read_certificate()
{
  ASN1_HEADER *as_header = NULL;
  BUF_MEM *bmem = NULL;
  BIO *cert_bio = NULL;

  unsigned char *p;
  unsigned char *op;
  int size = 0, i;

  if(!cert_file || !*cert_file)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  if(!in_format)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);

  if(!cert) {
    cert = X509_new();
    if(!cert) return(ACRYPTO_ERR_ALLOCATION_FAILURE);
  }

  if(!(cert_bio = BIO_new(BIO_s_file()))) {
    X509_free(cert);
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);
  }

  if(BIO_read_filename(cert_bio, cert_file) <= 0) {
    X509_free(cert);
    BIO_free_all(cert_bio);
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);
  }

  switch(in_format) {

  case FORMAT_ASN1:
    
    cert = d2i_X509_bio(cert_bio, NULL);
    
    break;

  case FORMAT_NET:

    if(!(bmem = BUF_MEM_new()))
       return(ACRYPTO_ERR_ALLOCATION_FAILURE);

    for(;;) {
      
      if(!BUF_MEM_grow(bmem, size + (1024 * 10))) 
       return(ACRYPTO_ERR_ALLOCATION_FAILURE);
      
     size += i = BIO_read(cert_bio, &(bmem->data[size]), (1024*10));

     if(!i) break;
     else if(i < 0) 
       return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);
    }

    op = p = (unsigned char *)bmem->data;

    /* Load the header */
    
    if(!(as_header = d2i_ASN1_HEADER(NULL, (const unsigned char **)&p, (long)size)))
      return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

    if((!as_header->header) || (!as_header->header->data) ||
       (strncmp(CERTIFICATE_HEADER, (char *)as_header->header->data, as_header->header->length))) 
      return(ACRYPTO_ERR_CERT_INVALID_CERT_HEADER);
      
    /* Since the header is ok, read the object */

    p = op;

    as_header->meth = X509_asn1_meth();
    
    if(!(as_header = d2i_ASN1_HEADER(&as_header, (const unsigned char **)&p, (long)size)))
      return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

    cert = (X509 *)as_header->data;
    as_header->data = NULL;

    break;

  case FORMAT_PEM:
    
    cert = PEM_read_bio_X509(cert_bio, NULL, NULL, NULL);
    
    break;
    
  default:
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);
  }

  if (as_header != NULL) ASN1_HEADER_free(as_header);
  if (cert_bio != NULL) BIO_free(cert_bio);
  if (bmem != NULL) BUF_MEM_free(bmem);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::read_certificate(const char *filename, int mode)
{
  if((!filename) || (!*filename))
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  if(!VALID_CERT_FORMAT(mode))
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);

  in_format = mode;

  cert_file = _strdup(filename);

  read_certificate();

  return(ACRYPTO_OK);
}

int ACryptoCertificate::read_certificate_request()
{
  EVP_PKEY *pubkey;
  X509_CINF *ci;
  BIO *req_in;
  
  if(!(req_in = BIO_new(BIO_s_file())))
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);

  if(BIO_read_filename(req_in, cert_req_file) <= 0)
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  if(!(cert_req = PEM_read_bio_X509_REQ(req_in, NULL, NULL, NULL))) {

    BIO_free(req_in);
    
    return(ACRYPTO_ERR_CERT_INVALID_CERT_REQ);
  }

  BIO_free(req_in);

  if(!cert_req->req_info ||
     !cert_req->req_info->pubkey ||
     !cert_req->req_info->pubkey->public_key ||
     !cert_req->req_info->pubkey->public_key->data)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_REQ);

  if(!(pubkey = X509_REQ_get_pubkey(cert_req)))
    return(ACRYPTO_ERR_CERT_INVALID_PUBKEY);

  if(X509_REQ_verify(cert_req, pubkey) < 1)
    return(ACRYPTO_ERR_CERT_INVALID_PUBKEY_VERIFY);

  if(cert) X509_free(cert);

  cert = X509_new();

  if(!cert) 
    return(ACRYPTO_ERR_CERT_INVALID_CERT);

  ci = cert->cert_info;

  /*  
      Umm...
      
      Something is very broken here, only I don't know quite what yet... :(
      
      if(!ASN1_INTEGER_set(X509_get_serialNumber(cert), 0)) 
      return(-1);
      
      if(!X509_set_issuer_name(cert, cert_req->req_info->subject)) 
      return(-1);
      
      if(!X509_set_subject_name(cert, cert_req->req_info->subject)) 
      return(-1);
      
      X509_gmtime_adj(X509_get_notBefore(cert), 0);
      
      X509_gmtime_adj(X509_get_notAfter(cert),(long)60*60*24*days);
      
      X509_PUBKEY_free(ci->key);
      
      ci->key = cert_req->req_info->pubkey;
      
      cert_req->req_info->pubkey = NULL;
  */

  return(ACRYPTO_OK);
}  

int ACryptoCertificate::read_private_key()
{
  BIO *key_bio = NULL;
  RSA *rsa = NULL;

  if(!key_file || !*key_file)
    return(ACRYPTO_ERR_CERT_INVALID_PRIVKEY_FILE);

  if(!(key_bio = BIO_new(BIO_s_file())))
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);

  if(BIO_read_filename(key_bio, key_file) <= 0) 
    return(ACRYPTO_ERR_CERT_INVALID_PRIVKEY_FILE);

  if(key_format == FORMAT_ASN1) {
    if(rsa = d2i_RSAPrivateKey_bio(key_bio, NULL))
      if(privkey = EVP_PKEY_new())
	EVP_PKEY_assign_RSA(privkey, rsa);
      else
	RSA_free(rsa);
  }
  else if(key_format == FORMAT_PEM) 
    privkey = PEM_read_bio_PrivateKey(key_bio, NULL, NULL, NULL);

  else return(ACRYPTO_ERR_CERT_INVALID_KEY_FORMAT);

  if(key_bio) BIO_free(key_bio);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::read_private_key(const char *keyfile, int format)
{
  BIO *key_bio = NULL;
  RSA *rsa = NULL;

  if(!keyfile || !*keyfile)
    return(ACRYPTO_ERR_CERT_INVALID_PRIVKEY_FILE);

  key_file = (char *)keyfile;
  key_format = format;
  
  if(!(key_bio = BIO_new(BIO_s_file())))
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);

  if(BIO_read_filename(key_bio, (char *)keyfile) <= 0) 
    return(ACRYPTO_ERR_CERT_INVALID_PRIVKEY_FILE);

  if(key_format == FORMAT_ASN1) {
    if(rsa = d2i_RSAPrivateKey_bio(key_bio, NULL))
      if(privkey = EVP_PKEY_new())
	EVP_PKEY_assign_RSA(privkey, rsa);
      else
	RSA_free(rsa);
  }
  else if(key_format == FORMAT_PEM) 
    privkey = PEM_read_bio_PrivateKey(key_bio, NULL, NULL, NULL);
  else return(ACRYPTO_ERR_CERT_INVALID_KEY_FORMAT);

  if(key_bio) BIO_free(key_bio);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::write_certificate()
{
  ASN1_HEADER as_header;
  ASN1_OCTET_STRING os;

  BIO *cert_bio = NULL;

  int i;

  if(!out_cert_file || !*out_cert_file)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  if(!out_format)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);

  if(!cert) 
    return(ACRYPTO_ERR_CERT_INVALID_CERT);

  if(!(cert_bio = BIO_new(BIO_s_file())))
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);

  if(BIO_write_filename(cert_bio, out_cert_file) <= 0)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  switch(out_format) {

  case FORMAT_ASN1:
    
    i = i2d_X509_bio(cert_bio, cert);

    break;

  case FORMAT_PEM:

    i = PEM_write_bio_X509(cert_bio, cert);

    break;

  case FORMAT_NET:

    os.data = (unsigned char *)CERTIFICATE_HEADER;

    os.length = strlen(CERTIFICATE_HEADER);

    as_header.header = &os;

    as_header.data = (char *)cert;

    as_header.meth = X509_asn1_meth();

    i = ASN1_i2d_bio((i2d_of_void *)i2d_ASN1_HEADER, cert_bio, (unsigned char *)&as_header);

    break;
  }

  return(ACRYPTO_OK);
}

int ACryptoCertificate::set_in_cert_filename(const char *fn)
{
  if(!fn || !*fn)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  if(cert_file) 
    free(cert_file);

  cert_file = _strdup(fn);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::set_out_cert_filename(const char *fn)
{
  if(!fn || !*fn)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  if(out_cert_file)
    free(out_cert_file);

  out_cert_file = _strdup(fn);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::set_certificate_request_filename(const char *cr)
{
  if(!cr || !*cr)
    return(ACRYPTO_ERR_CERT_INVALID_CERT_FILE);

  if(cert_req_file)
    free(cert_req_file);

  cert_req_file = _strdup(cr);

  return(ACRYPTO_OK);
}

int ACryptoCertificate::set_key_format(int kf)
{
  if((kf == FORMAT_DER) || (kf == FORMAT_PEM))
    key_format = kf;
  else key_format = FORMAT_INVALID;

  return((key_format == kf) ? ACRYPTO_OK : ACRYPTO_ERR_CERT_INVALID_KEY_FORMAT);
}

int ACryptoCertificate::set_in_cert_format(int c)
{
  if(VALID_CERT_FORMAT(c))
    in_format = c;
  else in_format = FORMAT_INVALID;;

  return((key_format == c) ? ACRYPTO_OK : ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);
}
  
int ACryptoCertificate::set_out_cert_format(int c)
{
  if(VALID_CERT_FORMAT(c)) 
    out_format = c;
  else out_format = FORMAT_INVALID;

  return((key_format == c) ? ACRYPTO_OK : ACRYPTO_ERR_CERT_INVALID_CERT_FORMAT);
}

int ACryptoCertificate::self_sign(int days)
{
  if(!m_digest)
    return(ACRYPTO_ERR_INVALID_BASE_DIGEST);

  if(!privkey)
    return(ACRYPTO_ERR_CERT_INVALID_PRIVKEY);

  if(days < 1)
    return(ACRYPTO_ERR_CERT_INVALID_DAYS);

  EVP_PKEY_copy_parameters(X509_get_pubkey(cert), privkey);
  EVP_PKEY_save_parameters(X509_get_pubkey(cert), 1);

  if(!X509_set_issuer_name(cert, X509_get_subject_name(cert)))
    return(ACRYPTO_ERR_CERT_INVALID_ISSUER_NAME);

  if((!(X509_gmtime_adj(X509_get_notBefore(cert), 0))) ||
     (!(X509_gmtime_adj(X509_get_notAfter(cert),(long)DAYS_TO_SECONDS(days)))))
    return(ACRYPTO_ERR_CERT_INVALID_GMTIME);

  if(!X509_set_pubkey(cert, privkey))
    return(ACRYPTO_ERR_CERT_INVALID_PUBKEY);

  if(!X509_sign(cert, privkey, m_digest))
  return(ACRYPTO_ERR_CERT_SIGN_FAILED);

  return(ACRYPTO_OK);
}

void ACryptoCertificate::print_cert()
{
  BIO *out;

  if(cert) {
    OBJ_create(X509_OBJ_ID, X509_OBJ_SN, X509_OBJ_LN);
    
    out = BIO_new(BIO_s_file());
    
    if(out) {
      BIO_set_fp(out, stdout, BIO_NOCLOSE);

      X509_print(out, cert);

      BIO_free(out);
    }
  }
}

void ACryptoCertificate::oneline()
{
  char *ptr;

  ptr = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);

  if(ptr && *ptr) {
    printf("%s\n", ptr);
    free(ptr);
  }
}



