/**
 * \file
 *         Bitset.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "bitset.h"

uint8_t nanotorrent_bitset_count(uint32_t x) {
	// Hacker's Delight, p. 66, Figure 5-2
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x0000003F;
}
