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

/**
 * Swarm state
 */
typedef enum nanotorrent_swarm_state {
	/**
	 * Initializing
	 */
	NANOTORRENT_SWARM_INIT,
	/**
	 * Left the swarm
	 */
	NANOTORRENT_SWARM_LEFT,
	/**
	 * Joining swarm
	 */
	NANOTORRENT_SWARM_JOINING,
	/**
	 * Joined swarm
	 */
	NANOTORRENT_SWARM_JOINED
} nanotorrent_swarm_state_t;

PROCESS_NAME(nanotorrent_swarm_process);
extern process_event_t nanotorrent_swarm_event;

#define nanotorrent_swarm_is_event(ev) \
	((ev) == nanotorrent_swarm_event)

void nanotorrent_swarm_start();
void nanotorrent_swarm_stop();

nanotorrent_swarm_state_t nanotorrent_swarm_state();
bool nanotorrent_swarm_can_join();
bool nanotorrent_swarm_is_joined();

nanotorrent_peer_info_t *nanotorrent_swarm_peek_peer();
nanotorrent_peer_info_t *nanotorrent_swarm_pop_peer();

void nanotorrent_swarm_join();
void nanotorrent_swarm_leave();
void nanotorrent_swarm_force_leave();
void nanotorrent_swarm_refresh();
void nanotorrent_swarm_complete();

#endif /* NANOTORRENT_SWARM_H_ */
