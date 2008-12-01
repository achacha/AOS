#ifndef INCLUDED__pchACrypto_HPP__
#define INCLUDED__pchACrypto_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "apiABase.hpp"
#include "apiACrypto.hpp"

#include "AString.hpp"
#include "AException.hpp"

//a_openSSL
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/asn1.h>
#include <openssl/rand.h>
#include <openssl/dsa.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/conf.h>

#endif //INCLUDED__pchACrypto_HPP__
