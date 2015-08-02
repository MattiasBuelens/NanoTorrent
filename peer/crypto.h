/**
 * \file
 *         Cryptographic functions
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_CRYPTO_H_
#define NANOTORRENT_CRYPTO_H_

#include "common.h"
#include "crypto/avr/crypto-impl.h"

#define SHA1_HASH_SIZE 20

/*
 * SHA1 digest
 */
typedef struct sha1_digest {
	uint8_t bytes[SHA1_HASH_SIZE];
} sha1_digest_t;

#define sha1_cmp(left, right) (memcmp(left, right, sizeof(sha1_digest_t)) == 0)

#define sha1_copy(dest, src) (*(dest) = *(src))

void sha1_print(const sha1_digest_t *digest);
void sha1_write(char *buffer, const sha1_digest_t *digest);

void sha1_reset(sha1_context_t *context);
bool sha1_add(sha1_context_t *context, const uint8_t *src, const size_t len);
bool sha1_result(sha1_context_t *context, sha1_digest_t *dest);

bool sha1_compute(const uint8_t *src, const size_t len, sha1_digest_t *dest);

#endif /* NANOTORRENT_CRYPTO_H_ */
