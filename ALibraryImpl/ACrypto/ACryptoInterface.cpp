/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchACrypto.hpp"
#include "ACryptoInterface.hpp"

ACryptoInterface::ACryptoInterface() {
  _init();
}

ACryptoInterface::ACryptoInterface(const char *cipher, const char *digest)
{
  _init();

  if(cipher && *cipher) m_cipher = const_cast<struct evp_cipher_st*>(EVP_get_cipherbyname((char *)cipher));
  if(digest && *digest) m_digest = const_cast<struct env_md_st*>(EVP_get_digestbyname((char *)digest));
}

ACryptoInterface::ACryptoInterface(EVP_CIPHER *evpcipher, EVP_MD *evpdigest)
{
  _init();

  m_cipher = evpcipher;
  m_digest = evpdigest;
}

ACryptoInterface::~ACryptoInterface()
{
  _destroy_key_iv();

  m_cipher = NULL;
  m_digest = NULL;
}

void ACryptoInterface::_allocate_key() 
{
  if(m_cipher) {

    if(m_key) {
      memset(m_key, 0, m_key_size);
	
      free(m_key);
      
      m_key = NULL;
    }
    
    m_key = (unsigned char *)malloc(EVP_MAX_KEY_LENGTH); 
    
    memset(m_key, 0, EVP_MAX_KEY_LENGTH); 
    
    m_key_size = m_cipher->key_len;
  }
}

void ACryptoInterface::_allocate_iv()
{  
  if(m_cipher) {
    if(m_iv) {
      memset(m_iv, 0, m_iv_size);
      
      free(m_iv);
      
      m_iv = NULL;
    }
    m_iv = (unsigned char *)malloc(EVP_MAX_IV_LENGTH);
    
    memset(m_iv, 0, EVP_MAX_IV_LENGTH);
    
    m_iv_size = m_cipher->iv_len;
  }
}

void ACryptoInterface::_destroy_key()
{
  clear_key();

  free(m_key);

  m_key = NULL;
}

void ACryptoInterface::_destroy_iv()
{
  clear_iv();

  free(m_iv);
    
  m_iv = NULL;
}

void ACryptoInterface::_destroy_key_iv()
{
  _destroy_key();
  _destroy_iv(); 
}

void ACryptoInterface::_init()
{
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();

  m_cipher = NULL;
  m_digest = NULL;

  m_key = NULL;
  m_iv = NULL;
}

int ACryptoInterface::_hex_to_uchar(const char *str, int x)
{
  int i, z;
  unsigned char c, *buf;

  if(!str || !*str || (x < 1))
    return(-1);

  if(x == ACRYPTO_KEY) {
    if(!m_key) _allocate_key();
    else memset(m_key, 0, m_cipher->key_len);
    buf = m_key;
  }
  else if(x == ACRYPTO_IV) {
    if(m_cipher->iv_len) {
      if(!m_iv) _allocate_iv();
      else memset(m_iv, 0, m_cipher->iv_len);
      buf = m_iv;
    }
  }
  else return(-1);

  z = strlen(str);

  for(i = 0; i < z; i++) {

    c = (unsigned char)(*str++);

    if(!c) break;

    if ((c >= '0') && (c <= '9'))
      c -= '0';
    else if ((c >= 'A') && (c <= 'F'))
      c = c - 'A' + 10;
    else if ((c >= 'a') && (c <= 'f'))
	  c = c - 'a' + 10;
    else return(-1);

    if(i&1) buf[i/2] |= c;
    else buf[i/2] = (c<<4);
  }

  return(1);
}

void ACryptoInterface::_uchar_to_hex(char **buf, int x)
{
  int i;

  if(!m_cipher || !buf || (x < 1)) return;

  if((x == 1) && m_key) {

    *buf = (char *)malloc((2 * m_cipher->key_len) + 1);

    for(i = 0; i < m_cipher->key_len; i++) 
    {
#if (_MSC_VER >= 1400)
      sprintf_s((*buf + (2 * i)), (2 * m_cipher->key_len) + 1, "%02X", m_key[i]);
#else
      sprintf((*buf + (2 * i)), "%02X", m_key[i]);
#endif
    }
  }
  if((x == 2) && m_iv && m_cipher->iv_len) {

    *buf = (char *)malloc((2 * m_cipher->iv_len) + 1);

    for(i = 0; i < m_cipher->iv_len; i++)
    {
#if (_MSC_VER >= 1400)
      sprintf_s((*buf + (2 * i)), (2 * m_cipher->iv_len) + 1, "%02X", m_iv[i]);
#else
      sprintf((*buf + (2 * i)), "%02X", m_iv[i]);
#endif
    }
  } 
}

int ACryptoInterface::set_cipherbyname(const char *cipher)
{
  if(m_cipher) m_cipher = NULL;
  
  if(!cipher || !*cipher ||
     (!(m_cipher = const_cast<struct evp_cipher_st*>(EVP_get_cipherbyname((char *)cipher)))))
    return(ACRYPTO_ERR_INVALID_BASE_CIPHER);

  return(ACRYPTO_OK);
}

int ACryptoInterface::set_digestbyname(const char *digest)
{
  if(m_digest) m_digest = NULL;

  if(!digest || !*digest ||
     (!(m_digest = const_cast<struct env_md_st*>(EVP_get_digestbyname((char *)digest)))))
    return(ACRYPTO_ERR_INVALID_BASE_DIGEST);
  
  return(ACRYPTO_OK);
}

int ACryptoInterface::set_cipherdigestbyname(const char *cipher, const char *digest)
{
  if(!cipher || !*cipher || !(set_cipherbyname(cipher)))
    return(ACRYPTO_ERR_INVALID_BASE_CIPHER);

  if(!digest || !*digest || !(set_digestbyname(digest)))
    return(ACRYPTO_ERR_INVALID_BASE_DIGEST);

  return(ACRYPTO_OK);
}

char *ACryptoInterface::get_cipherbyname()
{
  return(((m_cipher != NULL) ? const_cast<char*>(OBJ_nid2sn(EVP_CIPHER_nid(m_cipher))) : NULL));
}

char *ACryptoInterface::get_digestbyname()
{
  return(((m_cipher != NULL) ? const_cast<char*>(OBJ_nid2sn(EVP_MD_type(m_digest))) : NULL));
}

int ACryptoInterface::set_cipher(EVP_CIPHER *evpcipher)
{
  return((m_cipher = evpcipher) ? ACRYPTO_OK : ACRYPTO_ERR_INVALID_EVP_CIPHER);
}

int ACryptoInterface::set_digest(EVP_MD *evpdigest)
{
  return((m_digest = evpdigest) ? ACRYPTO_OK : ACRYPTO_ERR_INVALID_EVP_DIGEST);
}   

int ACryptoInterface::create_key(unsigned char *salt, unsigned char *data, int datalen, int count)
{
  if(!m_cipher)
    return(ACRYPTO_ERR_INVALID_BASE_CIPHER);

  if(!m_digest) 
    return(ACRYPTO_ERR_INVALID_BASE_DIGEST);

  if(m_key) {
    memset(m_key, 0, m_cipher->key_len);

    free(m_key);

    m_key = NULL;

    _allocate_key();
  }
  else _allocate_key();
    
  if(m_iv) {
    memset(m_iv, 0, m_cipher->iv_len);

    free(m_iv);

    m_iv = NULL;

    _allocate_iv();
  }
  else _allocate_iv();

  EVP_BytesToKey(m_cipher, 
		 m_digest, 
		 salt, 
		 data, 
		 datalen, 
		 count, 
		 m_key, 
		 m_iv);

  return(ACRYPTO_OK);
}

void ACryptoInterface::clear_key()
{
  if(m_cipher && m_key)
    memset(m_key, 0, m_cipher->key_len);
}

void ACryptoInterface::clear_iv()
{
  if(m_cipher && !m_cipher->iv_len && m_iv)
    memset(m_iv, 0, m_cipher->iv_len);
}

unsigned char *ACryptoInterface::get_key()
{
  unsigned char *key = NULL;

  if(m_key) {
    key = (unsigned char *)malloc(m_cipher->key_len);

    memcpy(key, m_key, m_cipher->key_len);
  }
  
  return key;
}

char *ACryptoInterface::get_hex_key()
{
  char *buf;

  _uchar_to_hex(&buf, ACRYPTO_KEY);

  return buf;
}

unsigned char *ACryptoInterface::get_iv()
{
  unsigned char *iv = NULL;

  if(m_iv && (m_cipher->iv_len > 0)) {

    iv = (unsigned char *)malloc(m_cipher->iv_len);

    memcpy(iv, m_iv, m_cipher->iv_len);
  }

  return iv;
}

char *ACryptoInterface::get_hex_iv()
{
  char *buf;

  _uchar_to_hex(&buf, ACRYPTO_IV);

  return buf;
}

int ACryptoInterface::set_key(const unsigned char *key, unsigned int length)
{
  if(!key || (length < 1) || (length != (unsigned int)m_cipher->key_len))
    return(ACRYPTO_ERR_INVALID_BASE_KEY);
  
  if(!m_key) m_key = (unsigned char *)malloc(length);

  memcpy(m_key, key, length);

  return(ACRYPTO_OK);
}

int ACryptoInterface::set_iv(const unsigned char *iv, unsigned int length)
{
  if(!iv || (length < 1) || (length != (unsigned int)m_cipher->iv_len))
    return(ACRYPTO_ERR_INVALID_BASE_IV);

  if(!m_iv) m_iv = (unsigned char *)malloc(length);
  
  memcpy(m_iv, iv, length);

  return(ACRYPTO_OK);
}

int ACryptoInterface::set_hex_key(const char *key)
{
  if(!key || !*key || !m_key || (strlen(key)/2) != (unsigned int)m_cipher->key_len)
    return(ACRYPTO_ERR_INVALID_BASE_KEY);

  _hex_to_uchar(key, ACRYPTO_KEY);
  
  return(ACRYPTO_OK);
}

int ACryptoInterface::set_hex_iv(const char *iv)
{
  if(!iv || !*iv || !m_iv || (strlen(iv)/2) != (unsigned int)m_cipher->iv_len)
    return(ACRYPTO_ERR_INVALID_BASE_IV);

  _hex_to_uchar(iv, ACRYPTO_IV);

  return(ACRYPTO_OK);
}
