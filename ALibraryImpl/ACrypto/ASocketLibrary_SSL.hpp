/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__ASocketLibrary_SSL_HPP__
#define INCLUDED__ASocketLibrary_SSL_HPP__

#include "apiACrypto.hpp"
#include "ASocketLibrary.hpp"

/*!
This object must exist to use SSL sockets
Making it a global object in scope of application of shared object/dll will work
(see example at the end of this file)
*/
class ACRYPTO_API ASocketLibrary_SSL : public ASocketLibrary
{
public:
  ASocketLibrary_SSL();
  ~ASocketLibrary_SSL();
};

#endif

/*
Usage example:

#include "ASocketLibrary_SSL.hpp"
#include "ASocketListener_SSL.hpp"
#include "AFile_Socket_SSL.hpp"

//When program exits, the destructor will deinitialize the library
ASocketLibrary_SSL g_ApplicationLibraryInstance;

int main()
{
  // You Code Here
}
*/