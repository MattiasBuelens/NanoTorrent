/**
 * \file
 *         Cryptographic implementations for AVR
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_CRYPTO_IMPL_H_
#define NANOTORRENT_CRYPTO_IMPL_H_

#include "crypto/avr/sha1.h"

/*
 * SHA1 context
 */
typedef struct sha1_context {
	/**
	 * Internal state
	 */
	sha1_ctx_t state;
	/**
	 * Next message block
	 */
	int_least16_t next_length;
	uint8_t next_block[SHA1_BLOCK_BYTES];
} sha1_context_t;

#endif /* NANOTORRENT_CRYPTO_IMPL_H_ */
