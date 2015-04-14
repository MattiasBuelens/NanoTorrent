/**
 * \file
 *         NanoTorrent process.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "state.h"
#include "swarm.h"
#include "peer.h"
#include "piece.h"

#include "uip-ds6.h"

#define NANOTORRENT_ADDR_POLL_PERIOD (1 * CLOCK_SECOND)
struct etimer addr_poll;

nanotorrent_torrent_state_t nanotorrent_state;

#define state (nanotorrent_state)

/*---------------------------------------------------------------------------*/
PROCESS(nanotorrent_process, "NanoTorrent process");
AUTOSTART_PROCESSES(&nanotorrent_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_BEGIN()

		printf("NanoTorrent\n");
		printf("===========\n");
		printf("\n");

		// Initialize descriptor
		uiplib_ip6addrconv("aaaa::1", &state.desc.tracker_ip);
		state.desc.tracker_port = 33333;
		state.desc.info.file_size = (1 << 12);
		state.desc.info.piece_size = (1 << 8);
		state.desc.info.num_pieces = (1 << 4);
		strncpy(state.file_name, "myprogram",
				NANOTORRENT_FILE_NAME_LENGTH - 1);

		printf("Initializing...\n");
		nanotorrent_piece_init();
		nanotorrent_peer_init();
		nanotorrent_swarm_init();

		// Wait until ready
		etimer_set(&addr_poll, NANOTORRENT_ADDR_POLL_PERIOD);
		while (!nanotorrent_swarm_is_ready()) {
			etimer_reset(&addr_poll);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&addr_poll));
		}

		// Join the swarm
		nanotorrent_swarm_join();

		printf("Shutting down...\n");
		nanotorrent_swarm_shutdown();
		nanotorrent_peer_shutdown();
		nanotorrent_piece_shutdown();

	PROCESS_END()
}
/*---------------------------------------------------------------------------*/
