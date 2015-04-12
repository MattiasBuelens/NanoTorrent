/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_PEER_H_
#define NANOTORRENT_PACK_PEER_H_

#include "../peer.h"

void nanotorrent_pack_peer_have(uint8_t **cur,
		const nanotorrent_peer_have_t *message);

void nanotorrent_unpack_peer_have(const uint8_t **cur,
		nanotorrent_peer_have_t *message);

void nanotorrent_pack_peer_data_header(uint8_t **cur,
		const nanotorrent_peer_data_header_t *message);

void nanotorrent_unpack_peer_data_header(const uint8_t **cur,
		nanotorrent_peer_data_header_t *message);

#endif /* NANOTORRENT_PACK_PEER_H_ */
