#ifndef INCLUDED__ACryptoAsymmetric_HPP__
#define INCLUDED__ACryptoAsymmetric_HPP__

#include "apiACrypto.hpp"
#include "ACryptoInterface.hpp"

class ACRYPTO_API ACryptoAsymmetric : public ACryptoInterface
{
public:

  ACryptoAsymmetric();

  ACryptoAsymmetric(int);
  ACryptoAsymmetric(RSA *);  
  ACryptoAsymmetric(DSA *);
  ACryptoAsymmetric(DH *);  
  
  virtual ~ACryptoAsymmetric();

  int set_RSA_padding_mode(int);
  int get_RSA_padding_mode() { return RSA_padding; }

  int set_asym_mode(int);
  int get_asym_mode() { return asym_mode; }

  int set_in_mode(int);
  int get_in_mode() { return in_mode; }

  int set_out_mode(int);
  int get_out_mode() { return out_mode; }

  void clear_edca() { eca = 0; dca = 0; }
  void clear_modes() { in_mode = 0; out_mode = 0; }

  void set_enc_cb_arg(int e) { eca = e; }
  void set_dec_cb_arg(int d) { dca = d; }

  int get_enc_cb_arg() { return eca; }
  int get_dec_cb_arg() { return dca; }

  int load_random_bytes(const char *, char = 0, long * = NULL);
  int load_random_bytes(ACryptoInterface &, long *);

  void set_asym_callback(void (*)(int, int, void *));
  void set_asym_callback(void (*)(int, int, void *), char *);
  void set_asym_callback_arg(char *);

  int generate_key_pair(unsigned int, int = 0, unsigned char * = NULL, unsigned int = 0);

  int read_public_key(const char *, int = 0, int = 0);
  int read_public_key(ACryptoInterface &, int = 0, int = 0);

  int read_private_key(const char *, int = 0, int = 0);
  int read_private_key(ACryptoInterface &, int = 0, int = 0);

  int write_public_key(unsigned char **, int = 0, int = 0);
  int write_public_key(ACryptoInterface &, int = 0, int = 0);
  int write_public_key(BIO *, int = 0, int = 0);

  int write_private_key(unsigned char **, int = 0, int = 0);
  int write_private_key(ACryptoInterface &, int = 0, int = 0);
  int write_private_key(BIO *, int = 0, int = 0);

  virtual void encrypt() { };

  int public_encrypt(int, unsigned char *, unsigned char **, long * = NULL);
  int private_encrypt(int, unsigned char *, unsigned char **, long * = NULL);

  int public_encrypt(ACryptoInterface&, ACryptoInterface&, long * = NULL);
  int private_encrypt(ACryptoInterface&, ACryptoInterface&, long * = NULL);

  virtual void decrypt() { };

  int public_decrypt(int, unsigned char *, unsigned char **, long * = NULL);
  int private_decrypt(int, unsigned char *, unsigned char **, long * = NULL);
  int public_decrypt(ACryptoInterface&, ACryptoInterface&, long * = NULL);  
  int private_decrypt(ACryptoInterface&, ACryptoInterface&, long * = NULL);

  int enable_RSA_blinding();
  int disable_RSA_blinding();
  int get_RSA_blinding() { return RSA_blinding; }

  virtual int read(void *, int) { return 0; }
  virtual int write(void *, int) { return 0; }

  virtual long get_buffer_size() { return 0; }

protected:

  EVP_PKEY *m_evp_pkey;

  int asym_mode, in_mode, out_mode;
  int eca, dca, RSA_padding, RSA_blinding;

  void (*gen_callback)(int, int, void *);
  char *gen_cb_arg;

  void _init();
};

  
#endif //INCLUDED__ACryptoAsymmetric_HPP__



