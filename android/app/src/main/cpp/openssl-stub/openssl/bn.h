/** Android OpenSSL stub — BoringSSL not in NDK r27 sysroot */
#pragma once
#ifdef WOWEE_NO_OPENSSL

#include <cstddef>
#include <cstdint>
#include <cstring>

// Forward declarations
struct bignum_st;
typedef struct bignum_st BIGNUM;
struct bn_ctx_st;
typedef struct bn_ctx_st BN_CTX;
struct evp_md_st;
typedef struct evp_md_st EVP_MD;

// BigNum stubs
inline BIGNUM* BN_new() { return nullptr; }
inline void BN_free(BIGNUM*) {}
inline void BN_clear_free(BIGNUM*) {}
inline BIGNUM* BN_dup(const BIGNUM*) { return nullptr; }
inline BN_CTX* BN_CTX_new() { return nullptr; }
inline void BN_CTX_free(BN_CTX*) {}
inline int BN_num_bytes(const BIGNUM*) { return 0; }
inline int BN_bn2bin(const BIGNUM*, unsigned char*) { return 0; }
inline BIGNUM* BN_bin2bn(const unsigned char*, int, BIGNUM*) { return nullptr; }
inline char* BN_bn2hex(const BIGNUM*) { return nullptr; }
inline char* BN_bn2dec(const BIGNUM*) { return nullptr; }
inline int BN_hex2bn(BIGNUM**, const char*) { return 0; }
inline int BN_dec2bn(BIGNUM**, const char*) { return 0; }
inline int BN_cmp(const BIGNUM*, const BIGNUM*) { return 0; }
inline int BN_is_zero(const BIGNUM*) { return 1; }
inline int BN_set_word(BIGNUM*, unsigned long) { return 1; }
inline int BN_add(BIGNUM*, const BIGNUM*, const BIGNUM*) { return 0; }
inline int BN_sub(BIGNUM*, const BIGNUM*, const BIGNUM*) { return 0; }
inline int BN_mul(BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*) { return 0; }
inline int BN_mod(BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*) { return 0; }
inline int BN_mod_exp(BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*) { return 0; }

// EVP stubs
inline const EVP_MD* EVP_sha1() { return nullptr; }
inline const EVP_MD* EVP_md5() { return nullptr; }
inline int EVP_Digest(const void*, size_t, unsigned char*, unsigned int*, const EVP_MD*, void*) { return 0; }

// HMAC stub
inline unsigned char* HMAC(const EVP_MD*, const void*, int, const unsigned char*, int, unsigned char*, unsigned int*) { return nullptr; }

// RAND stub
inline int RAND_bytes(unsigned char*, int) { return 1; }

#endif