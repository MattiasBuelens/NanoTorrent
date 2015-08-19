/**
 * \file
 *         NanoTorrent API.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "nanotorrent.h"
#include "peer.h"
#include "piece.h"
#include "piece-select.h"

#if NANOTORRENT_TRACKER
#include "swarm.h"
#endif

nanotorrent_torrent_state_t nanotorrent_state;
#define state (nanotorrent_state)

/**
 * Seed timer
 */
static struct etimer seed_timer;
static bool seed_timer_set;

void nanotorrent_start(nanotorrent_torrent_desc_t desc, const char *file_name) {
	// Initialize state
	memset(&state, 0, sizeof(state));
	state.desc = desc;
	strncpy(state.file_name, file_name, NANOTORRENT_FILE_NAME_LENGTH - 1);
	seed_timer_set = false;

	// Calculate info hash
	nanotorrent_torrent_info_hash(&state.desc.info, &state.info_hash);

	// Start process
	process_start(&nanotorrent_process, NULL);
}

void nanotorrent_stop() {
	process_exit(&nanotorrent_process);
}

void nanotorrent_init() {
	nanotorrent_piece_init();
	nanotorrent_select_init();
	nanotorrent_peer_start();
#if NANOTORRENT_TRACKER
	nanotorrent_swarm_start();
#endif
}

void nanotorrent_shutdown() {
#if NANOTORRENT_TRACKER
	nanotorrent_swarm_stop();
#endif
	nanotorrent_peer_stop();
	nanotorrent_piece_shutdown();
}

#if NANOTORRENT_TRACKER
void nanotorrent_connect_swarm() {
	nanotorrent_peer_info_t *peer;
	nanotorrent_peer_conn_t *conn;
	while ((peer = nanotorrent_swarm_peek_peer()) != NULL) {
		conn = nanotorrent_peer_connect(peer);
		if (conn == NULL) {
			break;
		}
		nanotorrent_swarm_pop_peer();
	}
}

bool nanotorrent_handle_swarm_event() {
	switch (nanotorrent_swarm_state()) {
	case NANOTORRENT_SWARM_JOINED:
		// Swarm was refreshed
		// Try connecting with new peers
		NOTE("Refreshing swarm");
		nanotorrent_connect_swarm();
		return true;
	case NANOTORRENT_SWARM_LEFT:
		ERROR("Lost connection with swarm");
		return false;
	default:
		ERROR("Unexpected swarm event");
		return false;
	}
}

#define nanotorrent_check_event(ev) \
	(nanotorrent_peer_is_event(ev) \
			|| nanotorrent_swarm_is_event(ev) \
			|| (seed_timer_set && etimer_expired(&seed_timer)))

#else

#define nanotorrent_check_event(ev) \
	(nanotorrent_peer_is_event(ev) \
			|| (seed_timer_set && etimer_expired(&seed_timer)))

#endif /* NANOTORRENT_TRACKER */

bool nanotorrent_keep_going() {
	if (!nanotorrent_piece_is_seed()) {
		// Still leeching
		return true;
	}
	if (NANOTORRENT_SEED_TIME < 0) {
		// Seed forever
		return true;
	}
	if (!seed_timer_set) {
		// Seed for a while
		etimer_set(&seed_timer, NANOTORRENT_SEED_TIME);
		seed_timer_set = true;
	}
	if (etimer_expired(&seed_timer)) {
		// Stop seeding
		return false;
	}
	return true;
}

PROCESS(nanotorrent_process, "NanoTorrent process");
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_shutdown())
	PROCESS_BEGIN()

		// Initialize
		nanotorrent_init();

#if NANOTORRENT_TRACKER

		// Wait until ready to join
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (!nanotorrent_swarm_is_joining()) {
			ERROR("Failed to start joining swarm");
			PROCESS_EXIT();
		}

		PRINTF("Joining swarm with tracker [");
		PRINT6ADDR(&state.desc.tracker_ip);
		PRINTF("]:%u\n", state.desc.tracker_port);

		// Wait until joined
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (!nanotorrent_swarm_is_joined()) {
			ERROR("Failed to join swarm");
			PROCESS_EXIT();
		}
		PRINTF("Joined the swarm\n");

		// Connect with peers from swarm
		nanotorrent_connect_swarm();

#endif /* NANOTORRENT_TRACKER */

		// Exchange pieces
		while (nanotorrent_keep_going()) {
#if NANOTORRENT_TRACKER
			// Handle swarm event
			if (nanotorrent_swarm_is_event(ev)
					&& !nanotorrent_handle_swarm_event()) {
				PROCESS_EXIT();
			}

			// Handle peer event
			if (nanotorrent_peer_is_event(ev)) {
				// Try to connect with more peers
				nanotorrent_connect_swarm();
			}
#endif /* NANOTORRENT_TRACKER */

			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_check_event(ev));
		}

	PROCESS_END()
}
