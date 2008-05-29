/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchACrypto.hpp"
#include "ACryptoBuffer.hpp"

#define GET_BUFSIZE(b)	BIO_ctrl(b, BIO_CTRL_INFO, 0, NULL)

ACryptoBuffer::ACryptoBuffer()
{
  bmem = BIO_new(BIO_s_mem());
}
  
ACryptoBuffer::ACryptoBuffer(void *buf, int length)
{
  bmem = BIO_new(BIO_s_mem());

  if(buf && length)
    BIO_write(bmem, (char *)buf, length);
}

ACryptoBuffer::~ACryptoBuffer()
{
  BIO_reset(bmem);

  BIO_free(bmem);
}

int ACryptoBuffer::set_buffer(void *buf, int size)
{
  if(!buf)
    return(ACRYPTO_ERR_DEST_PTR_NULL);

  if(size < 1)
    return(ACRYPTO_ERR_INVALID_DEST_SIZE);
    
  BIO_reset(bmem);
  
  return(BIO_write(bmem, (char *)buf, size));
}

int ACryptoBuffer::read(void *buf, int size)
{
  if(!buf)
    return(ACRYPTO_ERR_DEST_PTR_NULL);

  if(size < 1) 
    return(ACRYPTO_ERR_INVALID_DEST_SIZE);

  return(BIO_read(bmem, (char *)buf, size));
}

int ACryptoBuffer::read(ACryptoInterface& o, int size)
{
  char *buf;
  int x = 0, y = 0;

  long _buf_size;

  if(!(_buf_size = get_buffer_size())) 
    return(0);

  buf = (char *)malloc(_buf_size);

  memset(buf, 0, _buf_size);

  if(!size) {
    for(;;) {
      x = read(buf, _buf_size);

      if(x > 0) o.write(buf, x);
      else break;

      y += x;
    }
  }
  else {
    for(; y < size; y+= x) {
      x = read(buf, _buf_size);

      if(x > 0) o.write(buf, x);
      else break;
    }
  }

  memset(buf, 0, _buf_size);

  free(buf);

  return(y);
}

int ACryptoBuffer::write(char c)
{
  return(BIO_write(bmem, (char *)&c, 1));
}

int ACryptoBuffer::write(void *buf, int size)
{
  if(!buf)
    return(ACRYPTO_ERR_SRC_PTR_NULL);

  if(size < 1)
    return(ACRYPTO_ERR_INVALID_DEST_SIZE);

  return(BIO_write(bmem, (char *)buf, size));
}

int ACryptoBuffer::write(ACryptoInterface& i, int size)
{
  char *buf;
  int x = 0, y = 0;
  long _buf_size;

  if(!(_buf_size = get_buffer_size())) 
    return(0);

  buf = (char *)malloc(_buf_size);

  memset(buf, 0, _buf_size);

  if(!size) {

    for(;;) {
      
      x = i.read(buf, _buf_size);

      if(x > 0) write(buf, x);
      else break;

      y += x;
    }
  }
  else {
    for(; y < size; y+= x) {

      x = i.read(buf, _buf_size);

      if(x > 0) write(buf, x);
      else break;
    }
  }

  memset(buf, 0, _buf_size);

  free(buf);

  return(y);
}

long ACryptoBuffer::get_buffer_size()
{
  return GET_BUFSIZE(bmem);
}

unsigned char *ACryptoBuffer::get_buffer(long *size)
{
  unsigned char *temp = NULL;
  unsigned char *buf = NULL;

  int s;

  s = GET_BUFSIZE(bmem);

  if(s) {
    temp = (unsigned char *)malloc(s);
    buf = (unsigned char *)malloc(s);
    
    memset(temp, 0, s);
    memset(buf, 0, s);
    
    if((BIO_read(bmem, (char *)temp, s) != s) ||
       (BIO_write(bmem, (char *)temp, s) != s)) {
      
      memset(temp, 0, s);
      memset(buf, 0, s);
      
      free(temp);
      free(buf);
      
      return(NULL);
    }
    
    memcpy(buf, temp, s);
    
    if(size) *size = s;
    
    free(temp);
  }
  return(buf);
}

void ACryptoBuffer::encrypt_buffer() 
{
  BIO *benc = NULL, *bmn = NULL, *bt = NULL;

  unsigned char *buf = NULL;
  long bsize = 0;

  int i;

  if(m_cipher && (bsize = GET_BUFSIZE(bmem))) {
    
    buf = (unsigned char *)malloc(bsize);
    memset(buf, 0, bsize);

    benc = BIO_new(BIO_f_cipher());
    bmn = BIO_new(BIO_s_mem());

    BIO_set_cipher(benc, m_cipher, m_key, m_iv, ENCRYPT);
    bmn = BIO_push(benc, bmn);

    for(;;) {
      i = BIO_read(bmem, (char *)buf, bsize);
      if(i <= 0) break;
      else if (BIO_write(bmn, (char *)buf, i) != i) break;
    }

    BIO_flush(bmn);

    free(buf);

    bt = BIO_pop(bmn);

    if(bt) bmn = bt;

    BIO_free(benc);
    benc = NULL;

    BIO_free_all(bmem);
    bmem = bmn;
  }
};

void ACryptoBuffer::decrypt_buffer() 
{
  BIO *benc = NULL, *bmn = NULL, *bt = NULL;

  unsigned char	*buf = NULL;
  long bsize = 0;

  int i;

  if(m_cipher && (bsize = GET_BUFSIZE(bmem))) {
    
    buf = (unsigned char *)malloc(bsize);
    memset(buf, 0, bsize);

    benc = BIO_new(BIO_f_cipher());
    bmn = BIO_new(BIO_s_mem());

    BIO_set_cipher(benc, m_cipher, m_key, m_iv, DECRYPT);
    bmn = BIO_push(benc, bmn);

    for(;;) {
      i = BIO_read(bmem, (char *)buf, bsize);
      if(i <= 0) break;
      else if (BIO_write(bmn, (char *)buf, i) != i) break;
    }

    BIO_flush(bmn);

    free(buf);

    bt = BIO_pop(bmn);

    if(bt) bmn = bt;

    BIO_free(benc);
    benc = NULL;

    BIO_free_all(bmem);
    bmem = bmn;
  }
};

char *ACryptoBuffer::digest()
{
  BIO *bmd = NULL, *bn = NULL;
  char *dgst = NULL, *pptr = NULL;
  char tbuf[30];	// This is larger than any current digest
  int bsize, len, i;
  
  if(m_digest) {
    if(bsize = BIO_ctrl(bmem, BIO_CTRL_INFO, 0, (char *)&pptr)) {

      bmd = BIO_new(BIO_f_md());
      BIO_set_md(bmd, m_digest);
      
      bn = BIO_new(BIO_s_null());
      bn = BIO_push(bmd, bn);
      
      BIO_write(bn, pptr, bsize);
      
      memset(tbuf, 0, (sizeof(tbuf)/sizeof(char)));
      
      if(len = BIO_gets(bn, (char *)&tbuf[0], (sizeof(tbuf)/sizeof(unsigned char)))) {

	      dgst = (char *)malloc((2 * len) + 1);

	      memset(dgst, 0, ((2 * len) + 1));

	      for(i = 0; i < len; i++)
        {
#if (_MSC_VER >= 1400)
          sprintf_s(dgst + (2 * i), (2 * len) + 1, "%02x", (char)(tbuf[i] & 0x7F));
#else
          sprintf(dgst + (2 * i), "%02x", (char)(tbuf[i] & 0x7F));
#endif
        }
      }
    }
  }
 
  BIO_free_all(bn);

  memset(&tbuf, 0, sizeof(tbuf)/sizeof(char)); 
  
  return dgst;
}

char *ACryptoBuffer::digest(const unsigned char *buf, int length)
{
  if((buf && *buf) && (length > 0)) set_buffer((void *)buf, length);
  return(digest());
}

int ACryptoBuffer::push_cipher(EVP_CIPHER *ec, int mode)
{
  BIO *bt;
  
  if(!ec)
    return(ACRYPTO_ERR_INVALID_EVP_CIPHER);

  if(!VALID_MODE(mode))
    return(ACRYPTO_ERR_INVALID_CIPHER_MODE);
    
  bt = BIO_new(BIO_f_cipher());
  
  BIO_set_cipher(bt, ec, m_key, m_iv, mode);
  
  bmem = BIO_push(bt, bmem);
  
  return(ACRYPTO_OK);
}

int ACryptoBuffer::push_cipher(const char *cname, int mode)
{
  BIO			*bt;
  EVP_CIPHER		*cipher;

  if(!cname || !*cname ||
     (!(cipher = const_cast<struct evp_cipher_st*>(EVP_get_cipherbyname((char *)cname)))))
    return(ACRYPTO_ERR_INVALID_CIPHER_NAME);
  
  if(!VALID_MODE(mode))
    return(ACRYPTO_ERR_INVALID_CIPHER_MODE);
  
  if(!(cipher = const_cast<struct evp_cipher_st*>(EVP_get_cipherbyname((char *)cname))))
    return(ACRYPTO_ERR_INVALID_CIPHER_NAME);
  
  bt = BIO_new(BIO_f_cipher());
  
  BIO_set_cipher(bt, cipher, m_key, m_iv, mode);
  
  bmem = BIO_push(bt, bmem);
  
  return(ACRYPTO_OK);
}

int ACryptoBuffer::push_cipher(ACryptoInterface& iwc, int mode)
{
  BIO *bt;

  EVP_CIPHER *cipher;

  unsigned char *key;
  unsigned char	*iv;

  if(!VALID_MODE(mode))
    return(ACRYPTO_OK);
  
  if(!(key = iwc.get_key()))
    return(ACRYPTO_ERR_INVALID_BASE_KEY);
  
  if(!(iv = iwc.get_iv())) {
    free(key);
    return(ACRYPTO_ERR_INVALID_BASE_IV);
  }

  if(!(cipher = iwc.get_cipher())) {
    free(key);
    free(iv);
    return(ACRYPTO_ERR_INVALID_BASE_CIPHER);
  }

  bt = BIO_new(BIO_f_cipher());
  
  BIO_set_cipher(bt, cipher, key, iv, mode);
  
  bmem = BIO_push(bt, bmem);
  
  free(key);
  free(iv);
  
  return(ACRYPTO_OK);
}
