#pragma once
#ifdef WOWEE_NO_OPENSSL
#include "evp.h"
struct evp_md_st; typedef struct evp_md_st EVP_MD;
inline unsigned char* HMAC(const EVP_MD*, const void*, int, const unsigned char*, int, unsigned char*, unsigned int*) { return nullptr; }
#endif
