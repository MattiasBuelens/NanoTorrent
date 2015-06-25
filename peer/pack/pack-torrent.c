/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-torrent.h"
#include "pack-basic.h"

uint8_t *nanotorrent_pack_torrent_info(uint8_t *buf,
		const nanotorrent_torrent_info_t *info) {
	buf = nanotorrent_pack_uint16(buf, &info->file_size);
	buf = nanotorrent_pack_uint16(buf, &info->piece_size);
	buf = nanotorrent_pack_uint8(buf, &info->num_pieces);

	int i;
	for (i = 0; i < info->num_pieces; i++) {
		buf = nanotorrent_pack_sha1_digest(buf, &info->piece_hashes[i]);
	}

	return buf;
}

const uint8_t *nanotorrent_unpack_torrent_info(const uint8_t *buf,
		nanotorrent_torrent_info_t *info) {
	buf = nanotorrent_unpack_uint16(buf, &info->file_size);
	buf = nanotorrent_unpack_uint16(buf, &info->piece_size);
	buf = nanotorrent_unpack_uint8(buf, &info->num_pieces);

	info->num_pieces = MIN(info->num_pieces, NANOTORRENT_MAX_PIECES);
	int i;
	for (i = 0; i < info->num_pieces; i++) {
		buf = nanotorrent_unpack_sha1_digest(buf, &info->piece_hashes[i]);
	}

	return buf;
}

uint8_t *nanotorrent_pack_torrent_desc(uint8_t *buf,
		const nanotorrent_torrent_desc_t *desc) {
	buf = nanotorrent_pack_ip6addr(buf, &desc->tracker_ip);
	buf = nanotorrent_pack_uint16(buf, &desc->tracker_port);
	buf = nanotorrent_pack_torrent_info(buf, &desc->info);
	return buf;
}

const uint8_t *nanotorrent_unpack_torrent_desc(const uint8_t *buf,
		nanotorrent_torrent_desc_t *desc) {
	buf = nanotorrent_unpack_ip6addr(buf, &desc->tracker_ip);
	buf = nanotorrent_unpack_uint16(buf, &desc->tracker_port);
	buf = nanotorrent_unpack_torrent_info(buf, &desc->info);
	return buf;
}
