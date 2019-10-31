#include "gtest/gtest.h"
#include <openssl/ssl.h>

TEST(ssl, normal)
{
    SSL_library_init();
    const SSL_METHOD *method = SSLv23_client_method();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    SSL_CTX *ctx = SSL_CTX_new(method);
}