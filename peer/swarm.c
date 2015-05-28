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

#include "lib/list.h"
#include "lib/memb.h"

#define state (nanotorrent_state)

process_event_t nanotorrent_swarm_event;

/**
 * Swarm state
 */
static nanotorrent_swarm_state_t swarm_state;

/**
 * Known peers
 */
LIST(peers);
MEMB(peer_memb, nanotorrent_peer_info_t, NANOTORRENT_MAX_SWARM_PEERS);

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
	// Initialize announce retrying
	nanotorrent_retry_init(&announce_retry, NANOTORRENT_ANNOUNCE_RETRY_TIMEOUT,
			nanotorrent_swarm_announce_retry);
	// Initialize state
	nanotorrent_swarm_leave_quiet();
	swarm_state = NANOTORRENT_SWARM_INIT;
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

void nanotorrent_swarm_set_ready() {
	if (swarm_state == NANOTORRENT_SWARM_INIT) {
		swarm_state = NANOTORRENT_SWARM_LEFT;
	}
}

nanotorrent_swarm_state_t nanotorrent_swarm_state() {
	return swarm_state;
}

bool nanotorrent_swarm_is_joining() {
	return swarm_state == NANOTORRENT_SWARM_JOINING;
}

bool nanotorrent_swarm_is_joined() {
	return swarm_state == NANOTORRENT_SWARM_JOINED;
}

bool nanotorrent_swarm_can_join() {
	return swarm_state == NANOTORRENT_SWARM_LEFT;
}

void nanotorrent_swarm_clear_peers() {
	list_init(peers);
	memb_init(&peer_memb);
}

nanotorrent_peer_info_t *nanotorrent_swarm_peek_peer() {
	return list_head(peers);
}

nanotorrent_peer_info_t *nanotorrent_swarm_pop_peer() {
	return list_pop(peers);
}

void nanotorrent_swarm_announce_send(nanotracker_announce_event_t event) {
	// Create announce request
	nanotorrent_announce_request_t request;
	sha1_copy(&request.info_hash, &state.info_hash);
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

#define nanotorrent_swarm_post_event() \
	process_post(&nanotorrent_process, nanotorrent_swarm_event, NULL)

void nanotorrent_swarm_join() {
	if (!nanotorrent_swarm_can_join()) {
		ERROR("Cannot join swarm, not ready yet");
		return;
	}

	// Announce started
	nanotorrent_swarm_announce_start(NANOTRACKER_ANNOUNCE_STARTED);
	// Notify joining
	swarm_state = NANOTORRENT_SWARM_JOINING;
	nanotorrent_swarm_post_event()
	;
}

void nanotorrent_swarm_leave_quiet() {
	// Mark as left
	swarm_state = NANOTORRENT_SWARM_LEFT;
	// Clear peers
	nanotorrent_swarm_clear_peers();
	// Stop periodic announce refresh
	etimer_stop(&refresh);
	// Stop announce retrying
	nanotorrent_swarm_announce_stop();
}

void nanotorrent_swarm_leave() {
	bool was_joined = (swarm_state == NANOTORRENT_SWARM_JOINING
			|| swarm_state == NANOTORRENT_SWARM_JOINED);
	nanotorrent_swarm_leave_quiet();
	if (was_joined) {
		// Announce leave
		nanotorrent_swarm_announce_send(NANOTRACKER_ANNOUNCE_STOPPED);
		// Notify left
		nanotorrent_swarm_post_event()
		;
	}
}

void nanotorrent_swarm_force_leave() {
	bool was_joined = nanotorrent_swarm_is_joined();
	nanotorrent_swarm_leave_quiet();
	if (was_joined) {
		// Notify left
		swarm_state = NANOTORRENT_SWARM_LEFT;
		nanotorrent_swarm_post_event()
		;
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
	swarm_state = NANOTORRENT_SWARM_JOINED;
	// Start periodic announce refresh
	etimer_set(&refresh, NANOTORRENT_ANNOUNCE_REFRESH_PERIOD);
	// Notify joined
	nanotorrent_swarm_post_event()
	;
}

void nanotorrent_swarm_handle_refresh() {
	// Restart periodic announce refresh
	etimer_set(&refresh, NANOTORRENT_ANNOUNCE_REFRESH_PERIOD);
	// Notify refreshed
	nanotorrent_swarm_post_event()
	;
}

void nanotorrent_swarm_handle_reply(struct udp_socket *tracker_socket,
		void *ptr, const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen) {
	nanotorrent_announce_reply_t reply;
	const uint8_t *cur = data;

	// Parse reply
	nanotorrent_unpack_announce_reply(&cur, &reply);

	// Compare torrent info hash
	if (!sha1_cmp(&state.info_hash, &reply.info_hash)) {
		WARN("Ignoring reply for unknown torrent");
		sha1_print(&reply.info_hash);
		PRINTF("\n");
		return;
	}

	// Replace peers
	nanotorrent_peer_info_t *peer;
	size_t i;
	nanotorrent_swarm_clear_peers();
	for (i = 0; i < reply.num_peers; i++) {
		peer = memb_alloc(&peer_memb);
		if (peer == NULL) {
			break;
		}
		nanotorrent_unpack_peer_info(&cur, peer);
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
		nanotorrent_swarm_set_ready();

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
				etimer_restart(&refresh);
				nanotorrent_swarm_refresh();
			}
			// Wait for retries and refreshes
			PROCESS_WAIT_EVENT_UNTIL(
					nanotorrent_retry_check(&announce_retry)
							|| etimer_expired(&refresh));
		}

	PROCESS_END()
}
