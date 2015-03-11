/**
 * \file
 *         Torrent management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_TORRENT_H_
#define NANOTORRENT_TORRENT_H_

#include "common.h"
#include "swarm.h"

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

/**
 * Torrent state
 */
typedef struct nanotorrent_torrent_state {
	/**
	 * Torrent descriptor
	 */
	nanotorrent_torrent_desc_t desc;
	/*
	 * Hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Port listening for connections from other peers
	 */
	uint16_t listen_port;
	/**
	 * Swarm state
	 */
	nanotorrent_swarm_state_t swarm;
} nanotorrent_torrent_state_t;

#endif /* NANOTORRENT_TORRENT_H_ */
