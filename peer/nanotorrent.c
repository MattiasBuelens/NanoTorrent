/**
 * \file
 *         NanoTorrent API.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "nanotorrent.h"
#include "swarm.h"
#include "peer.h"
#include "piece.h"
#include "piece-select.h"

nanotorrent_torrent_state_t nanotorrent_state;
#define state (nanotorrent_state)

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
	nanotorrent_peer_init();
	nanotorrent_select_init();
	nanotorrent_swarm_start();
}

void nanotorrent_shutdown() {
	nanotorrent_swarm_stop();
	nanotorrent_peer_shutdown();
	nanotorrent_piece_shutdown();
}

bool nanotorrent_handle_swarm_event() {
	switch (nanotorrent_swarm_state()) {
	case NANOTORRENT_SWARM_JOINED:
		// TODO Connect with peers
		return true;
	case NANOTORRENT_SWARM_LEFT:
		ERROR("Lost connection with swarm");
		return false;
	default:
		ERROR("Unexpected swarm event");
		return false;
	}
}

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

#define nanotorrent_check_event(ev) \
	(nanotorrent_swarm_is_event(ev) \
			|| (seed_timer_set && etimer_expired(&seed_timer)))

PROCESS(nanotorrent_process, "NanoTorrent process");
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_shutdown())
	PROCESS_BEGIN()

		// Initialize
		nanotorrent_init();

		// Wait until ready to join
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (!nanotorrent_swarm_can_join()) {
			ERROR("Failed to join swarm");
			PROCESS_EXIT()
			;
		}

		PRINTF("Joining swarm with tracker [");
		PRINT6ADDR(&state.desc.tracker_ip);
		PRINTF("]:%u\n", state.desc.tracker_port);

		// Wait until joined
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (!nanotorrent_swarm_is_joined()) {
			ERROR("Failed to join swarm");
			PROCESS_EXIT()
			;
		}
		PRINTF("Joined the swarm\n");

		// TODO Connect with peers

		// Exchange pieces
		while (nanotorrent_keep_going()) {
			// Handle swarm event
			if (nanotorrent_swarm_is_event(ev)
					&& !nanotorrent_handle_swarm_event()) {
				PROCESS_EXIT()
				;
			}

			// TODO Handle piece completion

			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_check_event(ev));
		}

	PROCESS_END()
}
