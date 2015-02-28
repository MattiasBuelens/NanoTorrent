/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "swarm.h"
#include "torrent.h"

void nanotorrent_swarm_handle_reply(uint8_t *data,
		nanotorrent_torrent_state_t *state) {
	nanotorrent_announce_reply_t *reply;
	nanotorrent_peer_info_t peer_infos[];

	// Parse reply
	reply = (nanotorrent_announce_reply_t *) data;

	// Compare torrent info hash
	if (!SHA1Equal(&state->info_hash, &reply->info_hash)) {
		PRINTF("Ignoring reply for unknown torrent ");
		SHA1Print(&reply->info_hash);
		PRINTF("\n");
		return;
	}

	// Update number of peers
	state->swarm.num_peers = MIN(reply->num_peers, NANOTORRENT_MAX_PEERS);
	// Read peers into state
	peer_infos = (nanotorrent_peer_info_t *)(reply + 1);
	for (int i = 0; i < state->swarm.num_peers; i++) {
		state->swarm.peers[i] = peer_infos[i];
	}
}
