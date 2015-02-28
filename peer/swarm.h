/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_SWARM_H_
#define NANOTORRENT_SWARM_H_

#include "common.h"

/**
 * Swarm state
 */
typedef struct nanotorrent_swarm_state {
	uint8_t num_peers;
	nanotorrent_peer_info_t peers[NANOTORRENT_MAX_PEERS];
} nanotorrent_swarm_state_t;

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
 * Tracker announce events
 */
enum nanotracker_announce_event {
	/**
	 * Client refreshes its swarm membership
	 */
	REFRESH = 0,
	/**
	 * Client join the swarm
	 */
	STARTED,
	/**
	 * Client gracefully leaves the swarm
	 */
	STOPPED,
	/**
	 * Client completed the download, becomes a seed
	 */
	COMPLETED
};
typedef uint8_t nanotracker_announce_event_t;

/**
 * Tracker announce request
 */
typedef struct nanotorrent_announce_request {
	/**
	 * SHA1 hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Peer connection info
	 */
	nanotorrent_peer_info_t peer_info;
	/**
	 * Number of other peers wanted
	 */
	uint8_t num_want;
	/**
	 * Event that triggered the request
	 */
	nanotracker_announce_event_t event;
} nanotorrent_announce_request_t;

/**
 * Tracker announce reply
 */
typedef struct nanotorrent_announce_reply {
	/**
	 * SHA1 hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Number of peers in reply
	 */
	uint8_t num_peers;
} nanotorrent_announce_reply_t;

#endif /* NANOTORRENT_SWARM_H_ */
