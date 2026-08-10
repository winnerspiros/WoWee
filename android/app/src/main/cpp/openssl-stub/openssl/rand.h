#pragma once
#ifdef WOWEE_NO_OPENSSL
inline int RAND_bytes(unsigned char*, int) { return 1; }
#endif
