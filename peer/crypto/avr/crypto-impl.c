/**
 * \file
 *         Cryptographic implementations for AVR
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "crypto/avr/crypto-impl.h"

void sha1_reset(sha1_context_t *context) {
	sha1_init(&context->state);
	context->next_length = 0;
}

bool sha1_add(sha1_context_t *context, const uint8_t *src, size_t len) {
	// First fill up next block if non-empty
	if (context->next_length != 0) {
		// Append data to next block buffer
		int_least16_t copy_len = MIN(len,
				SHA1_BLOCK_BYTES - context->next_length);
		memcpy(&context->next_block[context->next_length], src, copy_len);
		context->next_length += copy_len;
		src += copy_len;
		len -= copy_len;
		// If next block is complete
		if (context->next_length == SHA1_BLOCK_BYTES) {
			// Process next block
			sha1_nextBlock(&context->state, context->next_block);
			context->next_length = 0;
		}
	}
	// Process full blocks
	while (len >= SHA1_BLOCK_BYTES) {
		sha1_nextBlock(&context->state, src);
		src += SHA1_BLOCK_BYTES;
		len -= SHA1_BLOCK_BYTES;
	}
	// Store remainder in next block buffer
	if (len > 0) {
		memcpy(context->next_block, src, len);
		context->next_length = len;
	}
	return true;
}

bool sha1_result(sha1_context_t *context, sha1_digest_t *dest) {
	sha1_lastBlock(&context->state, context->next_block,
			context->next_length * 8);
	sha1_ctx2hash(&dest->bytes, &context->state);
	return true;
}
