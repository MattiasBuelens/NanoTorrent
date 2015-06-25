/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_TORRENT_H_
#define NANOTORRENT_PACK_TORRENT_H_

#include "../torrent.h"

uint8_t *nanotorrent_pack_torrent_info(uint8_t *buf,
		const nanotorrent_torrent_info_t *info);

const uint8_t *nanotorrent_unpack_torrent_info(const uint8_t *buf,
		nanotorrent_torrent_info_t *info);

uint8_t *nanotorrent_pack_torrent_desc(uint8_t *buf,
		const nanotorrent_torrent_desc_t *desc);

const uint8_t *nanotorrent_unpack_torrent_desc(const uint8_t *buf,
		nanotorrent_torrent_desc_t *desc);

#endif /* NANOTORRENT_PACK_TORRENT_H_ */
