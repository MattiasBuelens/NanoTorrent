/**
 * \file
 *         Bitset.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_BITSET_H_
#define NANOTORRENT_BITSET_H_

/**
 * Get a bit by its index
 */
#define nanotorrent_bitset_get(set, i) \
	((((set) >> (i)) & 1) == 1)

/**
 * Set the bit at the given index
 */
#define nanotorrent_bitset_set(set, i) \
	((set) |= (1 << (i)))

/**
 * Clear the bit at the given index
 */
#define nanotorrent_bitset_clear(set, i) \
	((set) &= ~(1 << (i)))

/**
 * Get the intersect of two bitsets
 *
 * (A ^ B) == { (x in A) and (x in B) }
 */
#define nanotorrent_bitset_intersect(a, b) \
	((a) & (b))

/**
 * Get the union of two bitsets
 *
 * (A U B) == { (x in A) or (x in B) }
 */
#define nanotorrent_bitset_union(a, b) \
	((a) | (b))

/**
 * Get the difference of two bitsets
 *
 * (A \ B) == { (x in A) and (x not in B) }
 */
#define nanotorrent_bitset_diff(a, b) \
	((a) & ~(b))

/**
 * Checks if all bits in B are also set in A
 *
 * (B => A) == { (x not in B) or (x in A) }
 *          == { not ((x in B) and (x not in A) }
 *          == (B \ A) is empty
 */
#define nanotorrent_bitset_contains(a, b) \
	(nanotorrent_bitset_diff(b, a) == 0)

/**
 * Count the number of set bits.
 */
uint8_t nanotorrent_bitset_count(uint32_t set);

#endif /* NANOTORRENT_BITSET_H_ */
