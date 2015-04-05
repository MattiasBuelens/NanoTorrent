/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-torrent.h"
#include "pack-basic.h"

void nanotorrent_pack_torrent_desc(uint8_t **cur,
		const nanotorrent_torrent_desc_t *desc) {
	nanotorrent_pack_ip6addr(cur, &desc->tracker_ip);
	nanotorrent_pack_uint16(cur, &desc->tracker_port);
	nanotorrent_pack_uint16(cur, &desc->file_size);
	nanotorrent_pack_uint16(cur, &desc->piece_size);
	nanotorrent_pack_uint16(cur, &desc->num_pieces);

	int i;
	for (i = 0; i < desc->num_pieces; i++) {
		nanotorrent_pack_sha1_digest(cur, &desc->piece_hashes[i]);
	}
}

void nanotorrent_unpack_torrent_desc(const uint8_t **cur,
		nanotorrent_torrent_desc_t *desc) {
	nanotorrent_unpack_ip6addr(cur, &desc->tracker_ip);
	nanotorrent_unpack_uint16(cur, &desc->tracker_port);
	nanotorrent_unpack_uint16(cur, &desc->file_size);
	nanotorrent_unpack_uint16(cur, &desc->piece_size);
	nanotorrent_unpack_uint16(cur, &desc->num_pieces);

	desc->num_pieces = MIN(desc->num_pieces, NANOTORRENT_MAX_PIECES);
	int i;
	for (i = 0; i < desc->num_pieces; i++) {
		nanotorrent_unpack_sha1_digest(cur, &desc->piece_hashes[i]);
	}
}
