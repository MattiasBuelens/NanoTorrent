/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "pack.h"
#include "swarm.h"
#include "torrent.h"

void nanotorrent_swarm_handle_reply(uint8_t *data,
		nanotorrent_torrent_state_t *state) {
	nanotorrent_announce_reply_t reply;

	// Parse reply
	nanotorrent_unpack_announce_reply(&data, &reply);

	// Compare torrent info hash
	if (!sha1_cmp(&state->info_hash, &reply->info_hash)) {
		PRINTF("Ignoring reply for unknown torrent ");
		sha1_print(&reply->info_hash);
		PRINTF("\n");
		return;
	}

	// Update number of peers
	state->swarm.num_peers = MIN(reply->num_peers, NANOTORRENT_MAX_PEERS);
	// Read peers into state
	for (int i = 0; i < state->swarm.num_peers; i++) {
		nanotorrent_pack_peer_info(&data, &state->swarm.peers[i]);
	}
}
