/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "swarm.h"
#include "pack.h"
#include "retry.h"

#define state (nanotorrent_state)

/**
 * UDP socket with tracker
 */
static struct udp_socket tracker_socket;

/**
 * Announce request retry
 */
static nanotorrent_retry_t announce_retry;
/**
 * Event of announce request currently trying to send
 */
static nanotracker_announce_event_t announce_retry_event;

/**
 * Timer until next announce refresh
 */
static struct etimer refresh;

void nanotorrent_swarm_handle_reply(struct udp_socket *tracker_socket,
		void *ptr, const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen);

void nanotorrent_swarm_announce_retry(nanotorrent_retry_event_t event);

void nanotorrent_swarm_init() {
	// Clear connected peers
	state.swarm.num_peers = 0;
	memset(&state.swarm.peers, 0, sizeof(state.swarm.peers));
	// Initialize announce retrying
	nanotorrent_retry_init(&announce_retry, NANOTORRENT_ANNOUNCE_RETRY_PERIOD,
			nanotorrent_swarm_announce_retry);
	// Initialize as having left the swarm
	nanotorrent_swarm_leave();
	// Register tracker socket
	udp_socket_close(&tracker_socket);
	udp_socket_register(&tracker_socket, NULL, nanotorrent_swarm_handle_reply);
}

void nanotorrent_swarm_shutdown() {
	// Leave swarm
	nanotorrent_swarm_leave();
	// Close tracker socket
	udp_socket_close(&tracker_socket);
}

bool nanotorrent_swarm_is_ready() {
	// Need a global IPv6 address before attempting to join
	if (uip_ds6_get_global(-1) == NULL) {
		return false;
	}
	return true;
}

bool nanotorrent_swarm_is_joined() {
	return state.swarm.is_joined;
}

void nanotorrent_swarm_set_joined(bool is_joined) {
	state.swarm.is_joined = is_joined;
}

void nanotorrent_swarm_announce_send(nanotracker_announce_event_t event) {
	// Get own global IPv6 address
	uip_ds6_addr_t *global_ds6_addr = uip_ds6_get_global(-1);
	if (global_ds6_addr == NULL) {
		ERROR("No global address");
		return;
	}

	// Create announce request
	nanotorrent_announce_request_t request;
	sha1_copy(&request.info_hash, &state.info_hash);
	uip_ip6addr_copy(&request.peer_info.peer_ip, &global_ds6_addr->ipaddr);
	request.peer_info.peer_port = state.listen_port;
	request.num_want = NANOTORRENT_MAX_PEERS;
	request.event = event;

	// Connect to tracker
	udp_socket_connect(&tracker_socket, &state.desc.tracker_ip,
	state.desc.tracker_port);

	// Pack request
	uint8_t data[sizeof(request)];
	uint8_t *cur = data;
	nanotorrent_pack_announce_request(&cur, &request);
	uint16_t len = cur - data;

	// Send request
	udp_socket_send(&tracker_socket, data, len);
}

void nanotorrent_swarm_announce_start(nanotracker_announce_event_t event) {
	announce_retry_event = event;
	nanotorrent_retry_start(&announce_retry, NANOTORRENT_MAX_ANNOUNCE_RETRIES);
}

void nanotorrent_swarm_announce_stop() {
	announce_retry_event = 0;
	nanotorrent_retry_stop(&announce_retry);
}

void nanotorrent_swarm_announce_retry(nanotorrent_retry_event_t event) {
	switch (event) {
	case RETRY_AGAIN:
		// Try again
		nanotorrent_swarm_announce_send(announce_retry_event);
		break;
	case RETRY_STOP:
		// Stopped retrying
		switch (announce_retry_event) {
		case NANOTRACKER_ANNOUNCE_STARTED:
			ERROR("Failed to join swarm");
			break;
		case NANOTRACKER_ANNOUNCE_REFRESH:
			WARN("Failed to refresh swarm");
			break;
		default:
			break;
		}
		// TODO Force leave
		break;
	}
}

void nanotorrent_swarm_join() {
	if (nanotorrent_swarm_is_joined()) {
		ERROR("Cannot join swarm, already in swarm");
		return;
	}

	nanotorrent_swarm_announce_start(NANOTRACKER_ANNOUNCE_STARTED);
}

void nanotorrent_swarm_leave() {
	if (nanotorrent_swarm_is_joined()) {
		PRINTF("Leaving swarm\n");
		// Mark as left
		nanotorrent_swarm_set_joined(false);
		// Announce leave
		nanotorrent_swarm_announce_send(NANOTRACKER_ANNOUNCE_STOPPED);
	}

	// Stop periodic announce refresh
	etimer_stop(&refresh);
	// Stop announce retrying
	nanotorrent_swarm_announce_stop();
}

void nanotorrent_swarm_refresh() {
	if (!nanotorrent_swarm_is_joined()) {
		ERROR("Cannot refresh swarm, not in swarm");
		return;
	}

	nanotorrent_swarm_announce_start(NANOTRACKER_ANNOUNCE_REFRESH);
}

void nanotorrent_swarm_complete() {
	if (!nanotorrent_swarm_is_joined()) {
		ERROR("Cannot notify completion to swarm, not in swarm");
		return;
	}

	nanotorrent_swarm_announce_send(NANOTRACKER_ANNOUNCE_COMPLETED);
}

void nanotorrent_swarm_handle_join() {
	// Mark as joined
	nanotorrent_swarm_set_joined(true);
	// Start periodic announce refresh
	etimer_restart(&refresh);
}

void nanotorrent_swarm_handle_reply(struct udp_socket *tracker_socket,
		void *ptr, const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen) {
	nanotorrent_announce_reply_t reply;
	const uint8_t *cur = data;
	int i;

	// Parse reply
	nanotorrent_unpack_announce_reply(&cur, &reply);

	// Compare torrent info hash
	if (!sha1_cmp(&state.info_hash, &reply.info_hash)) {
		WARN("Ignoring reply for unknown torrent");
		sha1_print(&reply.info_hash);
		PRINTF("\n");
		return;
	}

	// Update number of peers
	state.swarm.num_peers = MIN(reply.num_peers, NANOTORRENT_MAX_PEERS);
	// Read peers into state
	for (i = 0; i < state.swarm.num_peers; i++) {
		nanotorrent_unpack_peer_info(&cur, &state.swarm.peers[i]);
	}

	// Mark as joined
	switch (announce_retry_event) {
	case NANOTRACKER_ANNOUNCE_STARTED:
		// Successful join
		nanotorrent_swarm_handle_join();
		break;
	case NANOTRACKER_ANNOUNCE_REFRESH:
		// Successful refresh
		etimer_reset(&refresh);
		break;
	}

	// TODO Start connecting with peers
}

bool nanotorrent_swarm_check() {
	if (nanotorrent_retry_check(&announce_retry)) {
		return true;
	}
	if (etimer_expired(&refresh)) {
		return true;
	}
	return false;
}

void nanotorrent_swarm_process(process_event_t ev) {
	// Send announce request retries
	nanotorrent_retry_process(&announce_retry);
	// Send announce refresh periodically
	if (etimer_expired(&refresh)) {
		etimer_reset(&refresh);
		nanotorrent_swarm_refresh();
	}
}
