/**
 * \file
 *         NanoTorrent process.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "state.h"
#include "swarm.h"

static nanotorrent_torrent_state_t state;

/*---------------------------------------------------------------------------*/
PROCESS(nanotorrent_process, "NanoTorrent process");
AUTOSTART_PROCESSES(&nanotorrent_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_BEGIN()

		printf("NanoTorrent\n");

		// Initialize descriptor
		uip_ip6addr(&state.desc.tracker_ip, 0xaaaa, 0x0, 0x0, 0x0, 0x1, 0x2,
				0x3, 0x4);
		state.desc.tracker_port = 33333;
		state.desc.file_size = (1 << 12);
		state.desc.piece_size = (1 << 8);
		state.desc.num_pieces = (1 << 4);

		uip_init();

		nanotorrent_swarm_init(&state);

		// Join the swarm
		nanotorrent_swarm_join(&state);

	PROCESS_END()
}
/*---------------------------------------------------------------------------*/
