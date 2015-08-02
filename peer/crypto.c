/**
 * \file
 *         Cryptographic functions.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "crypto.h"

void sha1_print(const sha1_digest_t *digest) {
	int i;
	for (i = 0; i < SHA1_HASH_SIZE; i++) {
		PRINTF("%02x", digest->bytes[i]);
	}
}

void sha1_write(char *buffer, const sha1_digest_t *digest) {
	int i;
	for (i = 0; i < SHA1_HASH_SIZE; i++) {
		sprintf(&buffer[2 * i], "%02x", digest->bytes[i]);
	}
	buffer[2 * SHA1_HASH_SIZE] = 0;
}

bool sha1_compute(const uint8_t *src, const size_t len, sha1_digest_t *dest) {
	sha1_context_t context;
	sha1_reset(&context);
	sha1_add(&context, src, len);
	return sha1_result(&context, dest);
}
