/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchACrypto.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ASocketException.hpp"
#include "ALock.hpp"

ASocketLibrary_SSL::ASocketLibrary_SSL()
{
#if defined(__WINDOWS__)
  SSL_library_init();
  ERR_load_crypto_strings();
  SSL_load_error_strings(); 
#endif
}

ASocketLibrary_SSL::~ASocketLibrary_SSL()
{
}
