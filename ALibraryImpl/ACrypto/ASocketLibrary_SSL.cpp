/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchACrypto.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ASocketException.hpp"
#include "ALock.hpp"

ASocketLibrary_SSL::ASocketLibrary_SSL()
{
  SSL_library_init();
  ERR_load_crypto_strings();
  SSL_load_error_strings(); 
}

ASocketLibrary_SSL::~ASocketLibrary_SSL()
{
  // Have to make all these calls until OpenSSL adds one function to do it all
  // This accounts for memory leaks inherent in the library on shutdown
  // Not really essential since process is exiting but allows better tracking of other
  //  leaks without getting a flood of leaks from OpenSSL on shutdown
  ERR_remove_state(0);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  ERR_free_strings();
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
}
