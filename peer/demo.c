/**
 * \file
 *         NanoTorrent demonstration.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki.h"

#include "nanotorrent.h"
#include "fs.h"
#include "pack.h"

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

		// Read descriptor from static file
		nanotorrent_torrent_desc_t desc = { };
		uint8_t buffer[sizeof(desc)];
		nanotorrent_fs_read(nanotorrent_fs_root, buffer, sizeof(buffer));
		nanotorrent_unpack_torrent_desc(buffer, &desc);

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
