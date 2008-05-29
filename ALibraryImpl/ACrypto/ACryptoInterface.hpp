/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ACryptoInterface_HPP__
#define INCLUDED__ACryptoInterface_HPP__

#include "apiACrypto.hpp"
#include "ADebugDumpable.hpp"
#include "ACryptoConstants.hpp"

/*!
Cryptography object base
Based on extension to ssleay by Chris Zimman

-----------------------START: HPP cut/paste-----------------------------------
public:
  virtual long get_buffer_size();
  virtual int  read(void *, int);
  virtual int  write(void *, int);
  virtual void encrypt();
  virtual void decrypt();
-----------------------STOP: HPP cut/paste-----------------------------------
*/
class ACRYPTO_API ACryptoInterface : public ADebugDumpable
{
public:
  ACryptoInterface();
  ACryptoInterface(const char *, const char *);

  ACryptoInterface(EVP_CIPHER *, EVP_MD *);

  virtual ~ACryptoInterface();

  // Set cipher and digest modes by name
  int set_cipherbyname(const char *);
  int set_digestbyname(const char *);
  int set_cipherdigestbyname(const char *, const char *);

  char *get_cipherbyname();
  char *get_digestbyname();

  int set_cipher(EVP_CIPHER *);
  int set_digest(EVP_MD *);

  EVP_CIPHER *get_cipher() const { return m_cipher; }
  EVP_MD     *get_digest() const { return m_digest; }

  // Key management
  int create_key(unsigned char *, unsigned char *, int, int count = 1);
  
  void clear_key();
  void clear_iv();
  
  void clear_key_iv() { clear_key(); clear_iv(); }
  
  unsigned char *get_key();
  char *get_hex_key();

  unsigned int get_key_len() { return((m_cipher) ? (m_cipher->key_len) : 0); }

  unsigned char *get_iv();
  char *get_hex_iv();

  unsigned int get_iv_len() { return((m_cipher) ? (m_cipher->iv_len) : 0); }

  int set_key(const unsigned char *, unsigned int);
  int set_iv(const unsigned char *, unsigned int);

  int set_hex_key(const char *);
  int set_hex_iv(const char *);

  /*!
  ACryptoInterface
  */
  virtual long get_buffer_size() = 0;
  virtual int  read(void *, int) = 0;
  virtual int  write(void *, int) = 0;
  virtual void encrypt() = 0;
  virtual void decrypt() = 0;

protected:
  EVP_CIPHER  *m_cipher;
  EVP_MD  *m_digest;

  u1 *m_key;
  u1 *m_iv;

  size_t m_key_size;
  size_t m_iv_size;

  void _init();

  void _allocate_key();
  void _allocate_iv();

  void _destroy_key();
  void _destroy_iv();

  void _destroy_key_iv();

  int _hex_to_uchar(const char *, int);
  void _uchar_to_hex(char **, int);
};

#endif //INCLUDED__ACryptoInterface_HPP__
