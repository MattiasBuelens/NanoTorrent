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
		NANOTORRENT_SWARM_WAIT_EVENT(ev, data, NANOTORRENT_SWARM_JOINING);
		PRINTF("Joining swarm with tracker [");
		PRINT6ADDR(&state.desc.tracker_ip);
		PRINTF("]:%u\n", state.desc.tracker_port);

		// Wait until joined
		NANOTORRENT_SWARM_WAIT_EVENT(ev, data, NANOTORRENT_SWARM_JOINED);
		PRINTF("Joined the swarm\n");

		while (nanotorrent_swarm_is_joined()) {

			// TODO Connect with peers
			// TODO Piece selection and peer data requests

			PROCESS_YIELD();
		}

	PROCESS_END()
}
