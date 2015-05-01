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

/**
 * Timer for checking whether NanoTorrent is ready
 */
#define NANOTORRENT_READY_POLL_PERIOD (1 * CLOCK_SECOND)
static struct etimer ready_poll;

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
	nanotorrent_swarm_init();
}

void nanotorrent_shutdown() {
	nanotorrent_swarm_shutdown();
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

		// Wait until ready
		etimer_set(&ready_poll, NANOTORRENT_READY_POLL_PERIOD);
		while (!nanotorrent_is_ready()) {
			etimer_reset(&ready_poll);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&ready_poll));
		}

		// Join the swarm
		PRINTF("Joining swarm with tracker [");
		PRINT6ADDR(&state.desc.tracker_ip);
		PRINTF("]:%u\n", state.desc.tracker_port);

		nanotorrent_swarm_join();
		while (!nanotorrent_swarm_is_joined()) {
			nanotorrent_swarm_process(ev);
			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_check());
		}
		PRINTF("Joined the swarm");

		while (nanotorrent_swarm_is_joined()) {
			nanotorrent_swarm_process(ev);

			// TODO Connect with peers
			// TODO Piece selection and peer data requests

			PROCESS_WAIT_EVENT_UNTIL(nanotorrent_swarm_check());
		}

		// Shutdown
		nanotorrent_shutdown();

	PROCESS_END()
}
