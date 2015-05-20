/**
 * \file
 *         Swarm management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "swarm.h"
#include "pack.h"
#include "retry.h"
#include "nanotorrent.h"

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

/**
 * Timer for checking whether swarm is ready
 */
#define SWARM_READY_POLL_PERIOD (1 * CLOCK_SECOND)
static struct etimer ready_poll;

void nanotorrent_swarm_handle_reply(struct udp_socket *tracker_socket,
		void *ptr, const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen);

void nanotorrent_swarm_leave_quiet();
void nanotorrent_swarm_announce_retry(nanotorrent_retry_event_t event,
		void *data);

void nanotorrent_swarm_start() {
	process_start(&nanotorrent_swarm_process, NULL);
}

void nanotorrent_swarm_stop() {
	process_exit(&nanotorrent_swarm_process);
}

void nanotorrent_swarm_init() {
	// Clear connected peers
	state.swarm.num_peers = 0;
	memset(&state.swarm.peers, 0, sizeof(state.swarm.peers));
	// Initialize announce retrying
	nanotorrent_retry_init(&announce_retry, NANOTORRENT_ANNOUNCE_RETRY_TIMEOUT,
			nanotorrent_swarm_announce_retry);
	// Initialize as having left the swarm
	nanotorrent_swarm_leave_quiet();
	// Register tracker socket
	udp_socket_close(&tracker_socket);
	udp_socket_register(&tracker_socket, NULL, nanotorrent_swarm_handle_reply);
}

void nanotorrent_swarm_shutdown() {
	// Leave swarm
	nanotorrent_swarm_force_leave();
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
	request.num_want = NANOTORRENT_MAX_SWARM_PEERS;
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
	nanotorrent_retry_start(&announce_retry, NANOTORRENT_MAX_ANNOUNCE_RETRIES,
			(NULL));
}

void nanotorrent_swarm_announce_stop() {
	announce_retry_event = 0;
	nanotorrent_retry_stop(&announce_retry);
}

void nanotorrent_swarm_announce_retry(nanotorrent_retry_event_t event,
		void *data) {
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
		// Force leave
		nanotorrent_swarm_force_leave();
		break;
	}
}

void nanotorrent_swarm_post_event(nanotorrent_swarm_event_type_t type) {
	process_post_synch(&nanotorrent_process, nanotorrent_swarm_event, &type);
}

void nanotorrent_swarm_join() {
	if (nanotorrent_swarm_is_joined()) {
		ERROR("Cannot join swarm, already in swarm");
		return;
	}

	// Announce started
	nanotorrent_swarm_announce_start(NANOTRACKER_ANNOUNCE_STARTED);
	// Notify joining
	nanotorrent_swarm_post_event(NANOTORRENT_SWARM_JOINING);
}

void nanotorrent_swarm_leave_quiet() {
	// Mark as left
	nanotorrent_swarm_set_joined(false);
	// Stop periodic announce refresh
	etimer_stop(&refresh);
	// Stop announce retrying
	nanotorrent_swarm_announce_stop();
}

void nanotorrent_swarm_leave() {
	bool was_joined = nanotorrent_swarm_is_joined();
	nanotorrent_swarm_leave_quiet();
	if (was_joined) {
		// Announce leave
		nanotorrent_swarm_announce_send(NANOTRACKER_ANNOUNCE_STOPPED);
		// Notify left
		nanotorrent_swarm_post_event(NANOTORRENT_SWARM_LEFT);
	}
}

void nanotorrent_swarm_force_leave() {
	bool was_joined = nanotorrent_swarm_is_joined();
	nanotorrent_swarm_leave_quiet();
	if (was_joined) {
		// Notify left
		nanotorrent_swarm_post_event(NANOTORRENT_SWARM_LEFT);
	}
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
	// Notify joined
	nanotorrent_swarm_post_event(NANOTORRENT_SWARM_JOINED);
}

void nanotorrent_swarm_handle_refresh() {
	// Restart periodic announce refresh
	etimer_reset(&refresh);
	// Notify refreshed
	nanotorrent_swarm_post_event(NANOTORRENT_SWARM_REFRESHED);
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
	state.swarm.num_peers = MIN(reply.num_peers, NANOTORRENT_MAX_SWARM_PEERS);
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
		nanotorrent_swarm_handle_refresh();
		break;
	}
}

PROCESS(nanotorrent_swarm_process, "NanoTorrent swarm process");
PROCESS_THREAD(nanotorrent_swarm_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_swarm_shutdown())
	PROCESS_BEGIN()

		// Initialize
		nanotorrent_swarm_event = process_alloc_event();
		nanotorrent_swarm_init();

		// Wait until ready
		etimer_set(&ready_poll, SWARM_READY_POLL_PERIOD);
		while (!nanotorrent_swarm_is_ready()) {
			etimer_reset(&ready_poll);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&ready_poll));
		}

		// Join the swarm
		nanotorrent_swarm_join();
		while (!nanotorrent_swarm_is_joined()) {
			// Send announce request retries
			nanotorrent_retry_process(&announce_retry);
			// Wait for retries
			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_retry_check(&announce_retry));
		}

		while (nanotorrent_swarm_is_joined()) {
			// Send announce request retries
			nanotorrent_retry_process(&announce_retry);
			// Send announce refresh periodically
			if (etimer_expired(&refresh)) {
				etimer_reset(&refresh);
				nanotorrent_swarm_refresh();
			}
			// Wait for retries and refreshes
			PROCESS_WAIT_EVENT_UNTIL(
					nanotorrent_retry_check(&announce_retry)
							|| etimer_expired(&refresh));
		}

	PROCESS_END()
}
