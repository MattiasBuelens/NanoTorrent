/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_TORRENT_H_
#define NANOTORRENT_PACK_TORRENT_H_

#include "../torrent.h"

void nanotorrent_pack_torrent_info(uint8_t **cur,
		const nanotorrent_torrent_info_t *info);

void nanotorrent_unpack_torrent_info(const uint8_t **cur,
		nanotorrent_torrent_info_t *info);

void nanotorrent_pack_torrent_desc(uint8_t **cur,
		const nanotorrent_torrent_desc_t *desc);

void nanotorrent_unpack_torrent_desc(const uint8_t **cur,
		nanotorrent_torrent_desc_t *desc);

#endif /* NANOTORRENT_PACK_TORRENT_H_ */
