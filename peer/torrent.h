/**
 * \file
 *         Torrent management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_TORRENT_H_
#define NANOTORRENT_TORRENT_H_

#include "common.h"

/**
 * Torrent info
 */
typedef struct nanotorrent_torrent_info {
	/**
	 * Sizes
	 */
	uint16_t file_size;
	uint16_t piece_size;
	/**
	 * Pieces
	 */
	uint8_t num_pieces;
	sha1_digest_t piece_hashes[NANOTORRENT_MAX_PIECES];
} nanotorrent_torrent_info_t;

/**
 * Torrent descriptor
 */
typedef struct nanotorrent_torrent_desc {
	/**
	 * Tracker
	 */
	uip_ip6addr_t tracker_ip;
	uint16_t tracker_port;
	/**
	 * Info
	 */
	nanotorrent_torrent_info_t info;
} nanotorrent_torrent_desc_t;

bool nanotorrent_torrent_info_hash(const nanotorrent_torrent_info_t *info,
		sha1_digest_t *info_hash);

bool nanotorrent_torrent_info_create(nanotorrent_torrent_info_t *info, int file,
		uint16_t piece_size);

#endif /* NANOTORRENT_TORRENT_H_ */
