#pragma once
#ifdef WOWEE_NO_OPENSSL
#include <cstdint>
struct evp_md_st; typedef struct evp_md_st EVP_MD;
inline const EVP_MD* EVP_sha1() { return nullptr; }
inline const EVP_MD* EVP_md5() { return nullptr; }
inline int EVP_Digest(const void*, size_t, unsigned char*, unsigned int*, const EVP_MD*, void*) { return 0; }
#endif