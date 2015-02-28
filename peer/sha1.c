/**
 * \file
 *         SHA-1
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "sha1.h"

int SHA1Equal(sha1_digest_t *left, sha1_digest_t *right) {
	return memcmp(left, right, sizeof(sha1_digest_t));
}

void SHA1Print(sha1_digest_t *digest) {
	for (int i = 0; i < SHA1HashSize; i++) {
		PRINTF("%02x", digest->bytes[i]);
	}
}
