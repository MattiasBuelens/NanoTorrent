/**
 * \file
 *         NanoTorrent API.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_H_
#define NANOTORRENT_H_

#include "common.h"
#include "torrent.h"

PROCESS_NAME(nanotorrent_process);

void nanotorrent_start(nanotorrent_torrent_desc_t desc, const char *file_name);
void nanotorrent_stop();

#endif /* NANOTORRENT_H_ */
