/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_SWARM_H_
#define NANOTORRENT_SWARM_H_

#include "common.h"
#include "torrent.h"
#include "state.h"

/**
 * Tracker announce events
 */
enum nanotracker_announce_event {
	/**
	 * Client refreshes its swarm membership
	 */
	NANOTRACKER_ANNOUNCE_REFRESH = 0,
	/**
	 * Client join the swarm
	 */
	NANOTRACKER_ANNOUNCE_STARTED,
	/**
	 * Client gracefully leaves the swarm
	 */
	NANOTRACKER_ANNOUNCE_STOPPED,
	/**
	 * Client completed the download, becomes a seed
	 */
	NANOTRACKER_ANNOUNCE_COMPLETED
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

void nanotorrent_swarm_init();
void nanotorrent_swarm_shutdown();
bool nanotorrent_swarm_is_ready();

void nanotorrent_swarm_join();

#endif /* NANOTORRENT_SWARM_H_ */
