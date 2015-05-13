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
#include "retry.h"

/**
 * Peer connection info
 */
typedef struct nanotorrent_peer_info {
	/**
	 * IPv6 address
	 */
	uip_ip6addr_t peer_ip;
} nanotorrent_peer_info_t;

#define nanotorrent_peer_info_cmp(a, b) \
	(uip_ip6addr_cmp(&(a)->peer_ip, &(b)->peer_ip))

/**
 * Swarm state
 */
typedef struct nanotorrent_swarm_state {
	/**
	 * Number of known peers
	 */
	uint8_t num_peers;
	/**
	 * Known peers
	 */
	nanotorrent_peer_info_t peers[NANOTORRENT_MAX_PEERS];
	/**
	 * Whether this peer has joined the swarm
	 */
	bool is_joined;
} nanotorrent_swarm_state_t;

/**
 * Peer state
 */
typedef struct nanotorrent_peer_state {
	/**
	 * Whether this peer connection is active
	 */
	bool is_active;
	/**
	 * Peer info
	 */
	nanotorrent_peer_info_t peer_info;
	/**
	 * Bit vector of completed pieces in peer's file
	 */
	uint32_t have;
	/**
	 * Piece index of currently requested piece
	 */
	uint8_t piece_index;
	/**
	 * Offset in currently requested piece
	 */
	uint16_t piece_offset;
	/**
	 * Piece request retry
	 */
	nanotorrent_retry_t piece_retry;
} nanotorrent_peer_state_t;

#define NANOTORRENT_MAX_EXCHANGE_PEERS (NANOTORRENT_MAX_OUT_PEERS + NANOTORRENT_MAX_IN_PEERS)

/**
 * Peer exchange state
 */
typedef struct nanotorrent_exchange_state {
	union {
		/**
		 * All peers
		 */
		nanotorrent_peer_state_t all[NANOTORRENT_MAX_EXCHANGE_PEERS];
		struct {
			/**
			 * Outgoing peers
			 */
			nanotorrent_peer_state_t out[NANOTORRENT_MAX_OUT_PEERS];
			/**
			 * Incoming peers
			 */
			nanotorrent_peer_state_t in[NANOTORRENT_MAX_IN_PEERS];
		};
	} peers;
} nanotorrent_exchange_state_t;

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
	 * Torrent descriptor
	 */
	nanotorrent_torrent_desc_t desc;
	/**
	 * Swarm state
	 */
	nanotorrent_swarm_state_t swarm;
	/**
	 * Exchange state
	 */
	nanotorrent_exchange_state_t exchange;
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
