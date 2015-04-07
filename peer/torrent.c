/**
 * \file
 *         Torrent management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "torrent.h"
#include "pack.h"

bool nanotorrent_torrent_info_hash(const nanotorrent_torrent_info_t *info,
		sha1_digest_t *info_hash) {
	// Pack torrent info
	uint8_t data[sizeof(*info)];
	uint8_t *cur = data;
	nanotorrent_pack_torrent_info(&cur, info);
	size_t len = cur - data;

	// Calculate hash
	return sha1_compute(data, len, info_hash);
}
