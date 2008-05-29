/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchACrypto.hpp"
#include "ACryptoAsymmetric.hpp"

#define RSA_PADDING_SIZE	11

ACryptoAsymmetric::ACryptoAsymmetric() 
{ 
  asym_mode = 0;

  _init();
}

ACryptoAsymmetric::ACryptoAsymmetric(int mode)
{
  if(AS_VALID_MODE(mode)) {

    set_asym_mode(mode);

    _init();
  }
}

ACryptoAsymmetric::ACryptoAsymmetric(RSA *r)
{
  if(r) {
   
    _init();
    
    EVP_PKEY_assign_RSA(m_evp_pkey, r);

    asym_mode = RSA_MODE;
  }
}

ACryptoAsymmetric::ACryptoAsymmetric(DSA *d)
{
  if(d) {

    _init();

    EVP_PKEY_assign_DSA(m_evp_pkey, d);
    
    asym_mode = DSA_MODE;
  }
}

ACryptoAsymmetric::ACryptoAsymmetric(DH *dh)
{
  if(dh) {

    _init();
  
    EVP_PKEY_assign_DH(m_evp_pkey, dh);

    asym_mode = DH_MODE;
  }
}

ACryptoAsymmetric::~ACryptoAsymmetric()
{
  if(m_evp_pkey)
    EVP_PKEY_free(m_evp_pkey);
}

void ACryptoAsymmetric::_init()
{
  m_evp_pkey = EVP_PKEY_new();

  printf("Initialized m_evp_pkey\n");
  fflush(stdout);

  clear_edca();

  RSA_blinding = 0;
  RSA_padding = RSA_PKCS1_PADDING;
}

int ACryptoAsymmetric::set_RSA_padding_mode(int p)
{
  if(!PADDING_VALID_MODE(p))
    return(ACRYPTO_ERR_ASYM_INVALID_RSA_PUBLIC_ENCRYPT);
  
  RSA_padding = p; 

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::set_asym_mode(int mode)
{
  if(!AS_VALID_MODE(mode))
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!m_evp_pkey)
    return(ACRYPTO_ERR_ASYM_INVALID_CONTEXT);

  EVP_PKEY_free(m_evp_pkey);
  
  m_evp_pkey = EVP_PKEY_new();

  asym_mode = mode;

  switch(asym_mode) {

  case RSA_MODE:
    
    m_evp_pkey->type = EVP_PKEY_RSA;
    break;

  case DSA_MODE:
    
    m_evp_pkey->type = EVP_PKEY_RSA;
    break;

  case DH_MODE:
    m_evp_pkey->type = EVP_PKEY_RSA;
    break;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::set_in_mode(int mode)
{
  if(!ACRYPTO_CA_VALID_MODE(mode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);

  in_mode = mode;

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::set_out_mode(int mode)
{
  if(!ACRYPTO_CA_VALID_MODE(mode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
    
  out_mode = mode;

  return(out_mode);
}

int ACryptoAsymmetric::load_random_bytes(const char *filename, char delimeter, long *total)
{
  char *ptr, *f, *p = NULL, *n = NULL;
  int last;

  if(!filename) 
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  ptr = f = _strdup(filename);

  if(!delimeter) if(total) *total += RAND_load_file((char *)filename, 1024L * 1024L);
  else for(;;) {
    
    last = 0;
    
    for(p = f; *p && (*p != delimeter); p++);
    
    if(!*p) last = 1;
    else *p = '\0';
    
    n = f;
    
    f = p + 1;
    
    if(total) *total += RAND_load_file(n, 1024L * 1024L);
    if(last) break;
  }

  free(ptr);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::load_random_bytes(ACryptoInterface &iwcf, long *total)
{
  unsigned char *buf = NULL;
  int size = 0, count = 0;

  size = iwcf.get_buffer_size();

  if(size) {
    
    buf = (unsigned char *)malloc(size);

    do {
      count = iwcf.read((char *)buf, size);
      if(total) *total += count;

      RAND_seed((unsigned char *)buf, count);

    } while(count);

    memset(buf, 0, size);

    free(buf);
  }

  return(ACRYPTO_OK);
}

void ACryptoAsymmetric::set_asym_callback(void (*func)(int, int, void *))
{
  gen_callback = func;
}

void ACryptoAsymmetric::set_asym_callback(void (*func)(int, int, void *), char *cbarg)
{
  gen_callback = func;
  gen_cb_arg = cbarg;
}

void ACryptoAsymmetric::set_asym_callback_arg(char *cbarg)
{
  gen_cb_arg = cbarg;
}

int ACryptoAsymmetric::generate_key_pair(unsigned int bits, int args, unsigned char *seed, unsigned int seed_len)
{
  int param;

  if(!asym_mode)
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);

  if(bits <= 0)
    return(ACRYPTO_ERR_ASYM_INVALID_MODULUS_SIZE);

  if(!m_evp_pkey)
    return(ACRYPTO_ERR_ASYM_INVALID_CONTEXT);

  switch(asym_mode) {

  case RSA_MODE:

    if(args) {
      if((args == RSA_F4) || (args == RSA_3)) 
	param = args;

      else return(ACRYPTO_ERR_ASYM_INVALID_RSA_EXP_MODE);
    }
    else param = RSA_F4;

    if(m_evp_pkey && m_evp_pkey->pkey.rsa) 
      RSA_free(m_evp_pkey->pkey.rsa);
    
    m_evp_pkey->pkey.rsa = RSA_generate_key(bits, param, gen_callback, (char *)gen_cb_arg);

    if(!m_evp_pkey->pkey.rsa) 
      return(ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT);
    
    break;

  case DSA_MODE:
    
    if(m_evp_pkey && m_evp_pkey->pkey.dsa) 
      DSA_free(m_evp_pkey->pkey.dsa);
    
    m_evp_pkey->pkey.dsa = DSA_generate_parameters(bits, seed, seed_len, NULL, NULL, gen_callback, (char *)gen_cb_arg);
    
    if(!m_evp_pkey->pkey.dsa) 
      return(ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT);
    
    break;
    
  case DH_MODE:

   if(args) {

     if((args == DH_GENERATOR_2) || (args == DH_GENERATOR_5))
       param = args;

     else return(ACRYPTO_ERR_ASYM_INVALID_DH_EXP_MODE);
   }
   else param = 2;

   if(m_evp_pkey && m_evp_pkey->pkey.dh) 
     DH_free(m_evp_pkey->pkey.dh);

   m_evp_pkey->pkey.dh = DH_generate_parameters(bits, param, gen_callback, (char *)gen_cb_arg);

   if(!m_evp_pkey->pkey.dh) 
     return(ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT);

   break;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::read_public_key(const char *filename, int asmode, int cmode)
{
  BIO *bp;
  int x = ACRYPTO_OK;

  if(!filename || !*filename)
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
  
  bp = BIO_new(BIO_s_file());
  
  if(BIO_read_filename(bp, (char *)filename) <= 0) 
    return(ACRYPTO_ERR_INVALID_FILE_NAME);
  
  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      if(!(m_evp_pkey->pkey.rsa = PEM_read_bio_RSAPublicKey(bp, NULL, NULL, NULL))) 
	x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_DER:
      
      if(!(m_evp_pkey->pkey.rsa = d2i_RSAPublicKey_bio(bp, NULL))) 
	x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_NET:
      
      BUF_MEM *buf = NULL;
      unsigned char *p;
      int size = 0, i = 0;
      
      buf = BUF_MEM_new();
      
      for (;;) {

	if ((buf == NULL) || (!BUF_MEM_grow(buf, size + (1024*10)))) 
	  return(ACRYPTO_ERR_ALLOCATION_FAILURE);
	
	size += i = BIO_read(bp, &(buf->data[size]), 1024*10);
	
	if (i == 0) break;
	
	if (i < 0) {
	  BUF_MEM_free(buf);
	  x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
	  break;
	  }
      }
	
      p = (unsigned char *)buf->data;
      
      m_evp_pkey->pkey.rsa = (RSA *)d2i_Netscape_RSA(NULL, (const unsigned char **)&p, (long)size, NULL);
      
      BUF_MEM_free(buf);
      
      if(!m_evp_pkey->pkey.rsa) 
	ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
      
      break;
    }
    
    break;	/* RSA_MODE */
    
    case DH_MODE:

      switch(cmode) {      

      case ACRYPTO_CA_PEM:

	if(!(m_evp_pkey->pkey.dh = PEM_read_bio_DHparams(bp, NULL, NULL, NULL))) 
	  x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;

	break;

      case ACRYPTO_CA_DER:

	if(!(m_evp_pkey->pkey.dh = d2i_DHparams_bio(bp, NULL))) 
	  x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;

	break;

      case ACRYPTO_CA_NET:

	break;
      }

      break;	/* DH_MODE */

    case DSA_MODE:

      switch(cmode) {
	
      case ACRYPTO_CA_PEM:

	if(!(m_evp_pkey->pkey.dsa = PEM_read_bio_DSAPrivateKey(bp, NULL, NULL, NULL)))
	  x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;

	break;

      case ACRYPTO_CA_DER:

	if(!(m_evp_pkey->pkey.dsa = d2i_DSAPrivateKey_bio(bp, NULL)))
	  x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;

	break;

      case ACRYPTO_CA_NET:

	break;
      }

      break;	/* DSA_MODE */
    }
  
  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::read_public_key(ACryptoInterface &in, int asmode, int cmode)
{
  BIO *bmem;
  char *ibuf = NULL;
  long b = 0;
  int x = ACRYPTO_OK;

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
  
  bmem = BIO_new(BIO_s_mem());

  ibuf = (char *)malloc(8192);

  while(b = in.read(ibuf, 8192))
    BIO_write(bmem, (char *)ibuf, b);

  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      if(!(m_evp_pkey->pkey.rsa = PEM_read_bio_RSAPublicKey(bmem, NULL, NULL, NULL))) 
	x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
      
	break;

      case ACRYPTO_CA_DER:

	if(!(m_evp_pkey->pkey.rsa = d2i_RSAPublicKey_bio(bmem, NULL))) 
	  x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
	
	break;
	
    case ACRYPTO_CA_NET:
      
      BUF_MEM *buf = NULL;
      unsigned char *p;
      int size = 0, i = 0;
      
      buf = BUF_MEM_new();

      for (;;) {

	if ((buf == NULL) || (!BUF_MEM_grow(buf, size + (1024*10)))) 
	  return(ACRYPTO_ERR_ALLOCATION_FAILURE);

	size += i = BIO_read(bmem, &(buf->data[size]), 1024*10);
	
	if (i == 0) break;
	
	if (i < 0) {
	  BUF_MEM_free(buf);
	  return(ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT);
	}
      }
      
      p = (unsigned char *)buf->data;

      m_evp_pkey->pkey.rsa = (RSA *)d2i_Netscape_RSA(NULL, (const unsigned char **)&p, (long)size, NULL);
      
      BUF_MEM_free(buf);
      
      break;
    }

    break;	/* RSA_MODE */

  case DH_MODE:
    
    switch(cmode) {      
      
    case ACRYPTO_CA_PEM:
      
      if(!(m_evp_pkey->pkey.dh = PEM_read_bio_DHparams(bmem, NULL, NULL, NULL))) 
	x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_DER:
      
      if(!(m_evp_pkey->pkey.dh = d2i_DHparams_bio(bmem, NULL))) 
	x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_NET:

      break;
    }
    
    break;	/* DH_MODE */

  case DSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      if(!(m_evp_pkey->pkey.dsa = PEM_read_bio_DSAPrivateKey(bmem, NULL, NULL, NULL)))
	x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_DER:
      
      if(!(m_evp_pkey->pkey.dsa = d2i_DSAPrivateKey_bio(bmem, NULL)))
	x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_NET:
      
      break;
    }

    break;	/* DSA_MODE */

  }
  
  BIO_reset(bmem);
  BIO_free(bmem);

  memset(ibuf, 0, 8192);
  free(ibuf);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::read_private_key(const char *filename, int asmode, int cmode)
{
  int x = ACRYPTO_OK;
  BIO *bp;

  if(!filename || !*filename) 
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
    
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
  
  bp = BIO_new(BIO_s_file());
  
  if(BIO_read_filename(bp, (char *)filename) <= 0) 
    return(ACRYPTO_ERR_INVALID_FILE_NAME);

  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      if(!(m_evp_pkey->pkey.rsa = PEM_read_bio_RSAPrivateKey(bp, NULL, NULL, NULL)))
	x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
      
      break;
      
    case ACRYPTO_CA_DER:
      
      if(!(m_evp_pkey->pkey.rsa = d2i_RSAPrivateKey_bio(bp, NULL)))
	x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;

      break;

    case ACRYPTO_CA_NET:
      
      BUF_MEM *buf = NULL;
      unsigned char *p;
      int size = 0, i = 0;
      
      buf = BUF_MEM_new();
      
      for (;;) {

	if ((buf == NULL) || (!BUF_MEM_grow(buf, size + (1024*10))))
	  return(ACRYPTO_ERR_ALLOCATION_FAILURE);
			
	  size += i = BIO_read(bp, &(buf->data[size]), 1024*10);

	  if (i == 0) break;
	  
	  if (i < 0) {
	    BUF_MEM_free(buf);
	    m_evp_pkey->pkey.rsa = NULL;
	    x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
	    break;
	  }
	}
	
	p = (unsigned char *)buf->data;

	if(!(m_evp_pkey->pkey.rsa = (RSA *)d2i_Netscape_RSA(NULL, (const unsigned char **)&p, (long)size, NULL)))
	  x = ACRYPTO_ERR_ASYM_INVALID_RSA_CONTEXT;
	
	BUF_MEM_free(buf);
	
	break;
      }

    case DH_MODE:
      
      switch(cmode) {      
	
      case ACRYPTO_CA_PEM:
	
	if(!(m_evp_pkey->pkey.dh = PEM_read_bio_DHparams(bp, NULL, NULL, NULL))) 
	  x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;
	
	break;
	
      case ACRYPTO_CA_DER:
	
	if(!(m_evp_pkey->pkey.dh = d2i_DHparams_bio(bp, NULL))) 
	  x = ACRYPTO_ERR_ASYM_INVALID_DH_CONTEXT;
	
	break;
	
      case ACRYPTO_CA_NET:
	
	break;
      }
      
    case DSA_MODE:
      
      switch(cmode) {
	
      case ACRYPTO_CA_PEM:
	
	if(!(m_evp_pkey->pkey.dsa = PEM_read_bio_DSAPrivateKey(bp, NULL, NULL, NULL)))
	  x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;
	
	break;
	
      case ACRYPTO_CA_DER:
	
	if(!(m_evp_pkey->pkey.dsa = d2i_DSAPrivateKey_bio(bp, NULL)))
	  x = ACRYPTO_ERR_ASYM_INVALID_DSA_CONTEXT;
	
	break;
	
      case ACRYPTO_CA_NET:
	
	break;
      }
  }
  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::write_public_key(unsigned char **buf, int asmode, int cmode)
{
  BIO *bmem = NULL;
  char *p = NULL;
  long b = 0;

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
  
  if(!buf)
    return(ACRYPTO_ERR_DEST_PTR_NULL);
	       
  bmem = BIO_new(BIO_s_mem());
  
  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      PEM_write_bio_RSAPublicKey(bmem, m_evp_pkey->pkey.rsa);
      
      break;
      
    case ACRYPTO_CA_DER:
      
      i2d_RSAPublicKey_bio(bmem, m_evp_pkey->pkey.rsa);
      
      break;
      
    case ACRYPTO_CA_NET:
      
      break;
    }
    
    break;	/* RSA_MODE */
    
    // How to deal with DSA/DH mode??
    // The public/private key relationship is totally different
    
  case DH_MODE:
    
    break;	/* DH_MODE */
    
  case DSA_MODE:
    
    break;	/* DSA_MODE */
  }
  
  if(!(b = BIO_get_mem_ptr(bmem, &p)))
    return(ACRYPTO_ERR_DEST_PTR_NULL);
    
  *buf = (unsigned char *)malloc(b);
  memmove(*buf, p, b);
  
  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::write_public_key(ACryptoInterface &o, int asmode, int cmode)
{
  BIO *bmem;
  char *buf = NULL, *p = NULL;
  long b = 0;

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);

  bmem = BIO_new(BIO_s_mem());

  switch(asmode) {
  
  case RSA_MODE:
  
    PEM_write_bio_RSAPublicKey(bmem, m_evp_pkey->pkey.rsa);
    
    break;
    
  case DH_MODE:

    PEM_write_bio_DHparams(bmem, m_evp_pkey->pkey.dh);

    break;
  }

  b = BIO_get_mem_ptr(bmem, &p);

  if(!b)
    return(ACRYPTO_ERR_DEST_PTR_NULL);

  buf = (char *)malloc(b);
  
  memmove(buf, p, b);
  
  o.write(buf, b);

  free(buf);
  
  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::write_public_key(BIO *bp, int asmode, int cmode)
{
  if(!bp) 
    return(ACRYPTO_ERR_INVALID_BIO_PTR);

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);
  
  switch(asmode) {
    
  case RSA_MODE:
  
    PEM_write_bio_RSAPublicKey(bp, m_evp_pkey->pkey.rsa);
    
    break;

  case DH_MODE:

    PEM_write_bio_DHparams(bp, m_evp_pkey->pkey.dh);

    break;
  }

  return(ACRYPTO_OK);
} 

int ACryptoAsymmetric::write_private_key(unsigned char **buf, int asmode, int cmode)
{
  BIO *bmem = NULL;
  char *p = NULL;
  long b = 0;

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);

  if(!buf) 
    return(ACRYPTO_ERR_DEST_PTR_NULL);
    
  bmem = BIO_new(BIO_s_mem());
  
  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      PEM_write_bio_RSAPrivateKey(bmem, m_evp_pkey->pkey.rsa, m_cipher, m_key, 0, NULL, NULL);
      
      break;
      
    case ACRYPTO_CA_DER:
      
      i2d_RSAPrivateKey_bio(bmem, m_evp_pkey->pkey.rsa);
      
      break;
      
    case ACRYPTO_CA_NET:
      
      unsigned char *ptr,*pp;
      int i, size;
      
      i = 1;
      
      size = i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, NULL, NULL);
      
      if(!(ptr = (unsigned char *)malloc(size)))
	return(ACRYPTO_ERR_ALLOCATION_FAILURE);
      
      pp = ptr;
      
      i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, &ptr, NULL);
      
      BIO_write(bmem, (char *)pp, size);
      
      free(pp);
      
      break;
    }
    
    break;	/* RSA_MODE */
    
  case DSA_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      PEM_write_bio_DSAPrivateKey(bmem, m_evp_pkey->pkey.dsa, m_cipher, m_key, 0, NULL, NULL);
      
      break;
      
    case ACRYPTO_CA_DER:
      
      i2d_DSAPrivateKey_bio(bmem, m_evp_pkey->pkey.dsa);
      
      break;
      
    }
    
    break;	/* DSA_MODE */
    
  case DH_MODE:
    
    switch(cmode) {
      
    case ACRYPTO_CA_PEM:
      
      PEM_write_bio_DHparams(bmem, m_evp_pkey->pkey.dh);
      
      break;
      
    case ACRYPTO_CA_DER:
      
      d2i_DHparams_bio(bmem, &m_evp_pkey->pkey.dh);
      
      break;
    }
    
    break;	/* DH_MODE */
  }
  
  b = BIO_get_mem_ptr(bmem, &p);
    
  if(!(*buf = (unsigned char *)malloc(b)))
    return(ACRYPTO_ERR_ALLOCATION_FAILURE);
    
  memmove(*buf, p, b);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::write_private_key(ACryptoInterface &o, int asmode, int cmode)
{
  BIO *bmem;
  char *buf = NULL, *p = NULL;
  long b = 0;

  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = in_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_FILE_MODE);

  bmem = BIO_new(BIO_s_mem());

  switch(asmode) {
    
  case RSA_MODE:
    
    switch(cmode) {

    case ACRYPTO_CA_PEM:
      
      PEM_write_bio_RSAPrivateKey(bmem, m_evp_pkey->pkey.rsa, m_cipher, m_key, 0, NULL, NULL);
      
      break;
      
    case ACRYPTO_CA_DER:
      
      i2d_RSAPrivateKey_bio(bmem, m_evp_pkey->pkey.rsa);
      
      break;
      
    case ACRYPTO_CA_NET:
      
      unsigned char *ptr,*pp;
      int i, size;
      
      i = 1;
      
      size = i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, NULL, NULL);
      
      if(!(ptr = (unsigned char *)malloc(size)))
	return(ACRYPTO_ERR_ALLOCATION_FAILURE);
      
      pp = ptr;
      
      i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, &ptr, NULL);
      
      BIO_write(bmem, (char *)pp, size);
      
      free(pp);
      
      break;
    }
    
    break;	/* RSA_MODE */

  case DSA_MODE:

    switch(cmode) {

    case ACRYPTO_CA_PEM:

      PEM_write_bio_DSAPrivateKey(bmem, m_evp_pkey->pkey.dsa, m_cipher, m_key, 0, NULL, NULL);

      break;

    case ACRYPTO_CA_DER:

      i2d_DSAPrivateKey_bio(bmem, m_evp_pkey->pkey.dsa);
      
      break;
      
    }
    
  case DH_MODE:
    
    PEM_write_bio_DHparams(bmem, m_evp_pkey->pkey.dh);
    
    break;
    
  }
  
  b = BIO_get_mem_ptr(bmem, &p);

  if(!b) 
    return(ACRYPTO_ERR_DEST_PTR_NULL);

  buf = (char *)malloc(b);
  
  memmove(buf, p, b);
  
  o.write(buf, b);

  free(buf);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::write_private_key(BIO *bp, int asmode, int cmode)
{
  if(!asmode) asmode = asym_mode;
  else if(!AS_VALID_MODE(asmode)) 
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);
  
  if(!cmode) cmode = out_mode;
  else if(!ACRYPTO_CA_VALID_MODE(cmode))
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);

  switch(asmode) {

  case RSA_MODE:

    switch(cmode) {

    case ACRYPTO_CA_PEM:

      PEM_write_bio_RSAPrivateKey(bp, m_evp_pkey->pkey.rsa, m_cipher, m_key, 0, NULL, NULL);
      
      break;

    case ACRYPTO_CA_DER:

      i2d_RSAPrivateKey_bio(bp, m_evp_pkey->pkey.rsa);
      
      break;

    case ACRYPTO_CA_NET:

      unsigned char *ptr,*pp;
      int i, size;
      
      i = 1;
      
      size = i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, NULL, NULL);
      
      if(!(ptr = (unsigned char *)malloc(size)))
	return(ACRYPTO_ERR_ALLOCATION_FAILURE);
      
      pp = ptr;
      
      i2d_Netscape_RSA(m_evp_pkey->pkey.rsa, &ptr, NULL);
      
      BIO_write(bp, (char *)pp, size);
      
      free(pp);
      
      break;
    }

    break;	/* RSA_MODE */

  case DSA_MODE:

    switch(cmode) {

    case ACRYPTO_CA_PEM:

      PEM_write_bio_DSAPrivateKey(bp, m_evp_pkey->pkey.dsa, m_cipher, m_key, 0, NULL, NULL);

      break;
      
    case ACRYPTO_CA_DER:

      i2d_DSAPrivateKey_bio(bp, m_evp_pkey->pkey.dsa);

      break;
    }

    break;	/* DSA_MODE */

  case DH_MODE:

    switch(cmode) {

    case ACRYPTO_CA_PEM:

      PEM_write_bio_DHparams(bp, m_evp_pkey->pkey.dh);

      break;
    }

    break;	/* DH_MODE */
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::public_encrypt(int from_len, unsigned char *from, unsigned char **to, long *length)
{
  int obufsize, isize, rsize;
  int x, y, z;

  x = y = z = 0;

  if((from_len < 1))
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  if(!from)
    return(ACRYPTO_ERR_SRC_PTR_NULL);
    
  rsize = RSA_size(m_evp_pkey->pkey.rsa);

  x = rsize - RSA_PADDING_SIZE;
  
  if(from_len > x) {
    isize = x;
    obufsize = (((from_len / x) + 1) * rsize);
  }
  else {
    isize = from_len;
    obufsize = rsize;
  }
  
  *to = (unsigned char *)malloc(obufsize);
  memset(*to, 0, obufsize);
  
  for(;z < from_len;) {
    
    if((z + isize) > from_len) 
      isize = (from_len - z);
    
    if((y = RSA_public_encrypt(isize, (from + z), (*to + x), m_evp_pkey->pkey.rsa, RSA_padding)) == -1)
      return(ACRYPTO_ERR_ASYM_INVALID_RSA_PUBLIC_ENCRYPT);

    if(length) length += x;

    if(gen_callback) gen_callback(eca, 0, (char *)gen_cb_arg);
    
    z += isize;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::private_encrypt(int from_len, unsigned char *from, unsigned char **to, long *length)
{
  int obufsize, isize, rsize;
  int x, y, z;

  x = y = z = 0;

  if((from_len < 1))
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  if(!from)
    return(ACRYPTO_ERR_SRC_PTR_NULL);

  rsize = RSA_size(m_evp_pkey->pkey.rsa);
  
  x = rsize - RSA_PADDING_SIZE;
  
  if(from_len > x) {
    isize = x;
    obufsize = (((from_len / x) + 1) * rsize);
  }
  else {
    isize = from_len;
    obufsize = rsize;
  }
  
  *to = (unsigned char *)malloc(obufsize);
  memset(*to, 0, obufsize);
  
  for(;z < from_len;) {
    
    if((z + isize) > from_len) 
      isize = (from_len - z);
    
    if((y = RSA_private_encrypt(isize, (from + z), (*to + x), m_evp_pkey->pkey.rsa, RSA_padding)) == -1)
      return(ACRYPTO_ERR_ASYM_INVALID_RSA_PRIVATE_ENCRYPT);

    if(length) *length += y;

    if(gen_callback) gen_callback(eca, 0, (char *)gen_cb_arg);

    z += isize;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::public_encrypt(ACryptoInterface &i, ACryptoInterface &o, long *length)
{
  unsigned char *ibuf, *obuf;
  int r, w, x, z;
  int bsize, rsa_size;

  x = z = 0;
  
  bsize = i.get_buffer_size();

  if(bsize < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  rsa_size = RSA_size(m_evp_pkey->pkey.rsa);
    
  w = rsa_size - RSA_PADDING_SIZE;
  
  ibuf = (unsigned char *)malloc(w);
  memset(ibuf, 0, w);

  obuf = (unsigned char *)malloc(rsa_size);
  memset(obuf, 0, rsa_size);
  
  while(x = i.read((char *)ibuf, w)) {
    
    if(x > 0) {
      
      if((r = RSA_public_encrypt(x, ibuf, obuf, m_evp_pkey->pkey.rsa, RSA_padding)) == -1) {

	memset(ibuf, 0, w);
	memset(obuf, 0, rsa_size);
	
	free(ibuf);
	free(obuf);

	return(ACRYPTO_ERR_ASYM_INVALID_RSA_PUBLIC_ENCRYPT);
      }

      if(length) *length += r;

      if(gen_callback) gen_callback(eca, 0, (char *)gen_cb_arg);
      
      if(o.write((char *)obuf, r) == -1) break;
    }
    else break;
  }
  
  memset(ibuf, 0, w);
  free(ibuf);
  
  memset(obuf, 0, rsa_size);
  free(obuf);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::private_encrypt(ACryptoInterface &i, ACryptoInterface &o, long *length)
{
  unsigned char *ibuf, *obuf;
  int r, w, x, z;
  int bsize, rsa_size;

  x = z = 0;
  
  bsize = i.get_buffer_size();

  if(bsize < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  rsa_size = RSA_size(m_evp_pkey->pkey.rsa);
  
  w = rsa_size - RSA_PADDING_SIZE;
  
  ibuf = (unsigned char *)malloc(w);
  memset(ibuf, 0, w);
  
  obuf = (unsigned char *)malloc(rsa_size);
  memset(obuf, 0, rsa_size);
  
  while(x = i.read((char *)ibuf, w)) {
    
    if(x > 0) {
      
      if((r = RSA_private_encrypt(x, ibuf, obuf, m_evp_pkey->pkey.rsa, RSA_padding)) == -1) {

	memset(ibuf, 0, w);
	memset(obuf, 0, rsa_size);

	free(ibuf);
	free(obuf);

	return(ACRYPTO_ERR_ASYM_INVALID_RSA_PRIVATE_ENCRYPT);
      }

      if(length) *length += r;

      if(gen_callback) gen_callback(eca, 0, (char *)gen_cb_arg);
      
      if(o.write((char *)obuf, r) == -1) break;
    }
    else break;
  }
  
  memset(ibuf, 0, w);
  free(ibuf);
  
  memset(obuf, 0, rsa_size);
  free(obuf);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::public_decrypt(int from_len, unsigned char *from, unsigned char **to, long *length)
{
  int obufsize, isize, rsize, x, y, z;

  x = y = z = 0;

  if(from_len < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  if(!from)
    return(ACRYPTO_ERR_SRC_PTR_NULL);

  if(!to)
    return(ACRYPTO_ERR_DEST_PTR_NULL);
    
  rsize = RSA_size(m_evp_pkey->pkey.rsa);
  
  if(from_len > rsize) {
    isize = rsize;
    obufsize = (((from_len / rsize) + 1) * rsize);
  }
  else {
    isize = from_len;
    obufsize = rsize;
  }
  
  *to = (unsigned char *)malloc(obufsize);
  memset(*to, 0, obufsize);
  
  for(;z < from_len;) {
    
    if((z + isize) > from_len) 
      isize = (from_len - z);
    
    if((y = RSA_public_decrypt(isize, (from + z), (*to + x), m_evp_pkey->pkey.rsa, RSA_padding)) == -1) {

      memset(from, 0, from_len);
      memset(*to, 0, obufsize);

      free(*to);

      return(ACRYPTO_ERR_ASYM_INVALID_RSA_PUBLIC_DECRYPT);
    }

    if(length) *length += y;

    if(gen_callback) gen_callback(dca, 0, (char *)gen_cb_arg);

    z += isize;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::public_decrypt(ACryptoInterface &i, ACryptoInterface &o, long *length)
{
  unsigned char *ibuf, *obuf;
  int r, x, z;
  int bsize, rsa_size;

  x = z = 0;
  
  bsize = i.get_buffer_size();

  if(bsize < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  rsa_size = RSA_size(m_evp_pkey->pkey.rsa);
  
  ibuf = (unsigned char *)malloc(rsa_size);
  memset(ibuf, 0, rsa_size);
  
  obuf = (unsigned char *)malloc(rsa_size);
  memset(obuf, 0, rsa_size);
  
  while(x = i.read((char *)ibuf, rsa_size)) {
    
    if(x > 0) {
      
      if((r = RSA_public_decrypt(x, ibuf, obuf, m_evp_pkey->pkey.rsa, RSA_padding)) == -1) {
	
	memset(ibuf, 0, rsa_size);
	memset(obuf, 0, rsa_size);
	
	free(ibuf);
	free(obuf);
	
	return(ACRYPTO_ERR_ASYM_INVALID_RSA_PUBLIC_DECRYPT);
      }
      
      if(length) *length += r;	
      
      if(gen_callback) gen_callback(dca, 0, (char *)gen_cb_arg);
      
      if(o.write((char *)obuf, r) == -1) break;
    }
    else break;
  }
    
  memset(ibuf, 0, rsa_size);
  free(ibuf);
  
  memset(obuf, 0, rsa_size);
  free(obuf);
  
  return(ACRYPTO_OK); 
}

int ACryptoAsymmetric::private_decrypt(int from_len, unsigned char *from, unsigned char **to, long *length)
{
  int obufsize, isize, rsize, x, y, z;

  x = y = z = 0;

  if(from_len < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);
  
  if(!from)
    return(ACRYPTO_ERR_SRC_PTR_NULL);

  if(!to)
    return(ACRYPTO_ERR_DEST_PTR_NULL);
  
  rsize = RSA_size(m_evp_pkey->pkey.rsa);
  
  if(from_len > rsize) {
    isize = rsize;
    obufsize = (((from_len / rsize) + 1) * rsize);
  }
  else {
    isize = from_len;
    obufsize = rsize;
  }
  
  *to = (unsigned char *)malloc(obufsize);
  memset(*to, 0, obufsize);
  
  for(;z < from_len;) {
    
    if((z + isize) > from_len) 
      isize = (from_len - z);
    
    if((y = RSA_private_decrypt(isize, (from + z), (*to + x), m_evp_pkey->pkey.rsa, RSA_padding)) == -1)
      return(ACRYPTO_ERR_ASYM_INVALID_RSA_PRIVATE_DECRYPT);
    
    if(length) *length += y;

    if(gen_callback) gen_callback(dca, 0, (char *)gen_cb_arg);
    
    z += isize;
  }

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::private_decrypt(ACryptoInterface &i, ACryptoInterface &o, long *length)
{
  unsigned char *ibuf, *obuf;
  int r, w, x, z;
  int bsize, rsa_size;

  w = x = z = 0;
  
  bsize = i.get_buffer_size();

  if(bsize < 1)
    return(ACRYPTO_ERR_INVALID_SRC_SIZE);

  rsa_size = RSA_size(m_evp_pkey->pkey.rsa);
  
  ibuf = (unsigned char *)malloc(rsa_size);
  memset(ibuf, 0, rsa_size);
  
  obuf = (unsigned char *)malloc(rsa_size);
  memset(obuf, 0, rsa_size);
  
  while(x = i.read((char *)ibuf, rsa_size)) {
    
    if(x > 0) {
      
      if((r = RSA_private_decrypt(x, ibuf, obuf, m_evp_pkey->pkey.rsa, RSA_padding)) == -1) {
	
	memset(ibuf, 0, rsa_size);
	memset(obuf, 0, rsa_size);

	free(ibuf);
	free(obuf);

	return(ACRYPTO_ERR_ASYM_INVALID_RSA_PRIVATE_DECRYPT);
      }
      
      if(length) *length += r;
      
      if(gen_callback) gen_callback(dca, 0, (char *)gen_cb_arg);
      
      if(o.write((char *)obuf, r) == -1) break;
    }
    else break;
  }
  
  memset(ibuf, 0, rsa_size);
  free(ibuf);
  
  memset(obuf, 0, rsa_size);
  free(obuf);

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::enable_RSA_blinding()
{
  if(asym_mode != RSA_MODE)
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);

  if(!m_evp_pkey || !m_evp_pkey->pkey.rsa)
    return(ACRYPTO_ERR_ASYM_INVALID_CONTEXT);

  RSA_blinding_on(m_evp_pkey->pkey.rsa, NULL);
    
  RSA_blinding = 1;

  return(ACRYPTO_OK);
}

int ACryptoAsymmetric::disable_RSA_blinding()
{
  if(asym_mode != RSA_MODE)
    return(ACRYPTO_ERR_ASYM_INVALID_MODE);

  if(!m_evp_pkey || !m_evp_pkey->pkey.rsa)
    return(ACRYPTO_ERR_ASYM_INVALID_CONTEXT);

  RSA_blinding_off(m_evp_pkey->pkey.rsa);
  
  RSA_blinding = 0;

  return(ACRYPTO_OK);
}



