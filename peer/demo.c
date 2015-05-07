/**
 * \file
 *         NanoTorrent demonstration.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "nanotorrent.h"

/*---------------------------------------------------------------------------*/
PROCESS(nanotorrent_demo_process, "NanoTorrent demonstration");
AUTOSTART_PROCESSES(&nanotorrent_demo_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nanotorrent_demo_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_stop())
	PROCESS_BEGIN()

		printf("NanoTorrent\n");
		printf("===========\n");
		printf("\n");

		// Initialize descriptor
		nanotorrent_torrent_desc_t desc;
		uip_ip6addr(&desc.tracker_ip, 0xaaaa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
				0x1);
		desc.tracker_port = 33333;
		desc.info.file_size = (1 << 12);
		desc.info.piece_size = (1 << 8);
		desc.info.num_pieces = (1 << 4);
		// TODO Initialize piece hashes

		const char *file_name = "myprogram";

		nanotorrent_start(desc, file_name);

		// TODO For debugging
		while (1) {
			PROCESS_WAIT_EVENT()
			;
		}

	PROCESS_END()
}
/*---------------------------------------------------------------------------*/
