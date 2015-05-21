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
 * Peer connection
 */
typedef struct nanotorrent_peer_conn {
	/**
	 * Whether this peer connection is valid
	 */
	bool is_valid;
	/**
	 * Peer info
	 */
	nanotorrent_peer_info_t peer_info;
	/**
	 * Bit vector of completed pieces in peer's file
	 */
	uint32_t have;
	/**
	 * Heartbeat timer
	 */
	struct etimer heartbeat;
	/**
	 * Whether currently requesting a piece from this peer
	 */
	bool has_request;
	/**
	 * Piece index of requested piece
	 */
	uint8_t request_index;
	/**
	 * Offset in requested piece
	 */
	uint16_t request_offset;
	/**
	 * Request retry
	 */
	nanotorrent_retry_t request_retry;
} nanotorrent_peer_conn_t;

#define NANOTORRENT_MAX_EXCHANGE_PEERS (NANOTORRENT_MAX_OUT_PEERS + NANOTORRENT_MAX_IN_PEERS)

/**
 * Peer exchange state
 */
typedef struct nanotorrent_exchange_state {
	union {
		/**
		 * All peer connections
		 */
		nanotorrent_peer_conn_t all[NANOTORRENT_MAX_EXCHANGE_PEERS];
		struct {
			/**
			 * Outgoing peer connections
			 */
			nanotorrent_peer_conn_t out[NANOTORRENT_MAX_OUT_PEERS];
			/**
			 * Incoming peer connections
			 */
			nanotorrent_peer_conn_t in[NANOTORRENT_MAX_IN_PEERS];
		};
	} peers;
	/**
	 * Bitset of pending pieces
	 */
	uint32_t pending_pieces;
} nanotorrent_exchange_state_t;

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
	 * Exchange state
	 */
	nanotorrent_exchange_state_t exchange;
} nanotorrent_torrent_state_t;

/**
 * Global state.
 */
extern nanotorrent_torrent_state_t nanotorrent_state;

#endif /* NANOTORRENT_STATE_H_ */
