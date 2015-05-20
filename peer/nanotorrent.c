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

nanotorrent_torrent_state_t nanotorrent_state;
#define state (nanotorrent_state)

struct etimer seed_timer;

void nanotorrent_start(nanotorrent_torrent_desc_t desc, const char *file_name) {
	// Initialize state
	memset(&state, 0, sizeof(state));
	state.desc = desc;
	strncpy(state.file_name, file_name, NANOTORRENT_FILE_NAME_LENGTH - 1);

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
	nanotorrent_swarm_start();
}

void nanotorrent_shutdown() {
	nanotorrent_swarm_stop();
	nanotorrent_peer_shutdown();
	nanotorrent_piece_shutdown();
}

bool nanotorrent_is_ready() {
	return nanotorrent_swarm_is_ready();
}

PROCESS(nanotorrent_process, "NanoTorrent process");
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_shutdown())
	PROCESS_BEGIN()

		// Initialize
		nanotorrent_init();

		// Wait until joining
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (nanotorrent_swarm_event_type(data) != NANOTORRENT_SWARM_JOINING) {
			ERROR("Failed to join swarm");
			PROCESS_EXIT()
			;
		}

		PRINTF("Joining swarm with tracker [");
		PRINT6ADDR(&state.desc.tracker_ip);
		PRINTF("]:%u\n", state.desc.tracker_port);

		// Wait until joined
		PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		if (nanotorrent_swarm_event_type(data) != NANOTORRENT_SWARM_JOINED) {
			ERROR("Failed to join swarm");
			PROCESS_EXIT()
			;
		}
		PRINTF("Joined the swarm\n");

		// TODO Connect with peers

		// Leeching
		while (!nanotorrent_piece_is_seed()) {
			if (nanotorrent_swarm_is_event(ev)) {
				switch (nanotorrent_swarm_event_type(data)) {
				case NANOTORRENT_SWARM_REFRESHED:
					// TODO Connect with peers
					break;
				case NANOTORRENT_SWARM_LEFT:
					ERROR("Lost connection with swarm");
					PROCESS_EXIT()
					;
					break;
				default:
					ERROR("Unexpected swarm event");
					PROCESS_EXIT()
					;
				}
			}

			// TODO Wait for swarm event or piece completion
			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_is_event(ev));
		}

		// Seeding
		if (NANOTORRENT_SEED_TIME < 0) {
			// Seed forever
			while (true) {
				PROCESS_YIELD()
				;
			}
		} else {
			// Seed for a while
			etimer_set(&seed_timer, NANOTORRENT_SEED_TIME);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&seed_timer));
		}

	PROCESS_END()
}
