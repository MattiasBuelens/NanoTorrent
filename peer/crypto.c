/**
 * \file
 *         Cryptographic functions.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "crypto.h"

void sha1_print(const sha1_digest_t *digest) {
	int i;
	for (i = 0; i < SHA1HashSize; i++) {
		PRINTF("%02x", digest->bytes[i]);
	}
}

void sha1_init(sha1_context_t *context) {
	SHA1Reset(context);
}

int sha1_add(sha1_context_t *context, const uint8_t *src, const size_t len) {
	return SHA1Input(context, src, len) == 0;
}

int sha1_result(sha1_context_t *context, sha1_digest_t *dest) {
	return SHA1Result(context, dest->bytes) == 0;
}

int sha1_compute(const uint8_t *src, const size_t len, sha1_digest_t *dest) {
	SHA1Context context;
	sha1_init(&context);
	sha1_add(&context, src, len);
	return sha1_result(&context, dest);
}
