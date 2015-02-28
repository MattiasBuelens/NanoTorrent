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
 * Torrent state
 */
typedef struct nanotorrent_torrent_state {
	sha1_digest_t info_hash;
	nanotorrent_swarm_state_t swarm;
} nanotorrent_torrent_state_t;

#endif /* NANOTORRENT_TORRENT_H_ */
