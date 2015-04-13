/**
 * \file
 *         State management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_STATE_H_
#define NANOTORRENT_STATE_H_

#include "common.h"
#include "torrent.h"

/**
 * Peer connection info
 */
typedef struct nanotorrent_peer_info {
	/**
	 * IPv6 address
	 */
	uip_ip6addr_t peer_ip;
	/**
	 * Port listening for connections from other peers
	 */
	uint16_t peer_port;
} nanotorrent_peer_info_t;

/**
 * Swarm state
 */
typedef struct nanotorrent_swarm_state {
	/**
	 * Number of connected peers
	 */
	uint8_t num_peers;
	/**
	 * Connected peers
	 */
	nanotorrent_peer_info_t peers[NANOTORRENT_MAX_PEERS];
	/**
	 * UDP socket with tracker
	 */
	struct udp_socket tracker_socket;
} nanotorrent_swarm_state_t;

/**
 * Peer state
 */
typedef struct nanotorrent_peer_state {
	/**
	 * UDP socket with peers
	 */
	struct udp_socket peer_socket;
} nanotorrent_peer_state_t;

/**
 * Piece state
 */
typedef struct nanotorrent_piece_state {
	/**
	 * File descriptor
	 */
	int file;
	/**
	 * Bit vector of completed pieces in own file
	 */
	uint32_t have;
} nanotorrent_piece_state_t;

/**
 * Torrent state
 */
typedef struct nanotorrent_torrent_state {
	/*
	 * Hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Destination file name
	 */
	char file_name[NANOTORRENT_FILE_NAME_LENGTH];
	/**
	 * Port listening for connections from other peers
	 */
	uint16_t listen_port;
	/**
	 * Torrent descriptor
	 */
	nanotorrent_torrent_desc_t desc;
	/**
	 * Swarm state
	 */
	nanotorrent_swarm_state_t swarm;
	/**
	 * Peer state
	 */
	nanotorrent_peer_state_t peer;
	/**
	 * Piece state
	 */
	nanotorrent_piece_state_t piece;
} nanotorrent_torrent_state_t;

/**
 * Global state.
 */
extern nanotorrent_torrent_state_t nanotorrent_state;

#endif /* NANOTORRENT_STATE_H_ */
