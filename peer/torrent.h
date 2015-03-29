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
 * Torrent descriptor
 */
typedef struct nanotorrent_torrent_desc {
	/**
	 * Tracker
	 */
	uip_ip6addr_t tracker_ip;
	uint16_t tracker_port;
	/**
	 * Sizes
	 */
	uint16_t file_size;
	uint16_t piece_size;
	/**
	 * Pieces
	 */
	uint8_t num_pieces;
	sha1_digest_t *piece_hashes; // TODO Where to allocate?
} nanotorrent_torrent_desc_t;

#endif /* NANOTORRENT_TORRENT_H_ */
