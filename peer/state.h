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
	nanotorrent_peer_info_t peers[NANOTORRENT_MAX_SWARM_PEERS];
	/**
	 * Whether this peer has joined the swarm
	 */
	bool is_joined;
} nanotorrent_swarm_state_t;

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
} nanotorrent_peer_conn_t;

/**
 * Pending piece request
 */
typedef struct nanotorrent_piece_request {
	/**
	 * Whether this request is valid
	 */
	bool is_valid;
	/**
	 * Uploading peer
	 */
	nanotorrent_peer_info_t peer;
	/**
	 * Piece index of requested piece
	 */
	uint8_t index;
	/**
	 * Offset in requested piece
	 */
	uint16_t offset;
	/**
	 * Request retry
	 */
	nanotorrent_retry_t retry;
} nanotorrent_piece_request_t;

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
	 * Pending requests
	 */
	nanotorrent_piece_request_t requests[NANOTORRENT_MAX_PEER_REQUESTS];
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
