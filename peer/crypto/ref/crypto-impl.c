/**
 * \file
 *         Cryptographic reference implementations.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "crypto/ref/crypto-impl.h"

void sha1_reset(sha1_context_t *context) {
	SHA1Reset(context);
}

bool sha1_add(sha1_context_t *context, const uint8_t *src, const size_t len) {
	return SHA1Input(context, src, len) == 0;
}

bool sha1_result(sha1_context_t *context, sha1_digest_t *dest) {
	return SHA1Result(context, dest->bytes) == 0;
}
