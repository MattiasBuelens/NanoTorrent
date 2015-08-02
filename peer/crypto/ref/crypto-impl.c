/**
 * \file
 *         Cryptographic reference implementations.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "crypto-impl.h"

void sha1_init(sha1_context_t *context) {
	SHA1Reset(context);
}

bool sha1_add(sha1_context_t *context, const uint8_t *src, const size_t len) {
	return SHA1Input(context, src, len) == 0;
}

bool sha1_result(sha1_context_t *context, sha1_digest_t *dest) {
	return SHA1Result(context, dest->bytes) == 0;
}

bool sha1_compute(const uint8_t *src, const size_t len, sha1_digest_t *dest) {
	SHA1Context context;
	sha1_init(&context);
	sha1_add(&context, src, len);
	return sha1_result(&context, dest);
}
