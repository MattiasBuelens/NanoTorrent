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

nanotorrent_torrent_state_t nanotorrent_state;

#define state (&nanotorrent_state)

/*---------------------------------------------------------------------------*/
PROCESS(nanotorrent_process, "NanoTorrent process");
AUTOSTART_PROCESSES(&nanotorrent_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nanotorrent_process, ev, data) {
	PROCESS_BEGIN()

		printf("NanoTorrent\n");

		// Initialize descriptor
		uip_ip6addr(&state->desc.tracker_ip, 0xaaaa, 0x0, 0x0, 0x0, 0x1, 0x2,
				0x3, 0x4);
		state->desc.tracker_port = 33333;
		state->desc.info.file_size = (1 << 12);
		state->desc.info.piece_size = (1 << 8);
		state->desc.info.num_pieces = (1 << 4);
		strncpy(state->file_name, "myprogram",
				NANOTORRENT_FILE_NAME_LENGTH - 1);

		nanotorrent_piece_init();
		nanotorrent_peer_init();
		nanotorrent_swarm_init();

		// Join the swarm
		nanotorrent_swarm_join();

		nanotorrent_swarm_shutdown();
		nanotorrent_peer_shutdown();
		nanotorrent_piece_shutdown();

	PROCESS_END()
}
/*---------------------------------------------------------------------------*/
