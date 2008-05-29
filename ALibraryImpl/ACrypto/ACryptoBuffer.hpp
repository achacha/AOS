/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED_ACryptoBuffer_HPP__
#define INCLUDED_ACryptoBuffer_HPP__

#include "apiACrypto.hpp"
#include "ACryptoInterface.hpp"

class ACRYPTO_API ACryptoBuffer : public ACryptoInterface
{
public:

  ACryptoBuffer();
  ACryptoBuffer(void *, int);  

  virtual ~ACryptoBuffer();

  int set_buffer(void *, int);

  virtual long get_buffer_size();
  unsigned char *get_buffer(long *);

  void clear_buffer() { BIO_reset(bmem); }

  int read(void *, int);
  int read(ACryptoInterface&, int size = 0);

  int write(char c);
  int write(void *, int);
  int write(ACryptoInterface&, int size = 0);
  
  virtual void encrypt() { };
  virtual void decrypt() { };

  void encrypt_buffer();
  void decrypt_buffer();

  int push_cipher(EVP_CIPHER *, int);
  int push_cipher(const char *, int);
  int push_cipher(ACryptoInterface&, int);

  void pop_ciper() { bmem = BIO_pop(bmem); }
  void clear_stack();

  char *digest();
  char *digest(const unsigned char *, int);

private:
  BIO *bmem;
};


#endif // INCLUDED_ACryptoBuffer_HPP__
