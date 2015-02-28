/**
 * \file
 *         SHA-1
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_SHA1_H_
#define NANOTORRENT_SHA1_H_

#include "crypto/sha1.h"

/*
 * SHA1 digest
 */
typedef struct sha1_digest {
	uint8_t bytes[SHA1HashSize];
} sha1_digest_t;

int SHA1Equal(sha1_digest_t *left, sha1_digest_t *right);

#endif /* NANOTORRENT_SHA1_H_ */
