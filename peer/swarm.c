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

void nanotorrent_swarm_init(nanotorrent_torrent_state_t *state) {
	// Clear connected peers
	state->swarm.num_peers = 0;
	memset(&state->swarm.peers, 0, sizeof(state->swarm.peers));
	// Register tracker socket
	udp_socket_close(&state->swarm.tracker_socket);
	// TODO 'state' must be in static memory to be accessible in callback
	udp_socket_register(&state->swarm.tracker_socket, state,
			nanotorrent_swarm_handle_reply);
}

void nanotorrent_swarm_join(nanotorrent_torrent_state_t *state) {
	nanotorrent_announce_request_t request;

	// Create announce request
	sha1_copy(&request->info_hash, &state->info_hash);
	uip_gethostaddr(&request->peer_info.peer_ip);
	request->peer_info.peer_port = state->listen_port;
	request->num_want = NANOTORRENT_MAX_PEERS;
	request->event = NANOTRACKER_ANNOUNCE_STARTED;

	// Connect to tracker
	udp_socket_connect(&state->swarm.tracker_socket, &state->desc.tracker_ip,
			state->desc.tracker_port);

	// Pack request
	uint8_t data[sizeof(request)];
	uint8_t *cur = data;
	nanotorrent_pack_announce_request(&cur, &request);
	uint16_t len = cur - data;

	// Send request
	udp_socket_send(&state->swarm.tracker_socket, data, len);
}

void nanotorrent_swarm_handle_reply(struct udp_socket *tracker_socket,
		void *ptr, const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen) {
	nanotorrent_announce_reply_t reply;
	nanotorrent_torrent_state_t *state = ptr;
	uint8_t *cur = data;

	// Parse reply
	nanotorrent_unpack_announce_reply(&cur, &reply);

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
		nanotorrent_pack_peer_info(&cur, &state->swarm.peers[i]);
	}

	// Close tracker socket
	udp_socket_close(&state->swarm.tracker_socket);

	// TODO Start connecting with peers
}
