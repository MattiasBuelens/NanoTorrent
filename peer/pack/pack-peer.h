/**
 * \file
 *         Packing/unpacking of torrent descriptors.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_PEER_H_
#define NANOTORRENT_PACK_PEER_H_

#include "../peer.h"

void nanotorrent_pack_peer_message_header(uint8_t **cur,
		const nanotorrent_peer_message_header_t *header);

void nanotorrent_unpack_peer_message_header(const uint8_t **cur,
		nanotorrent_peer_message_header_t *header);

void nanotorrent_pack_peer_close(uint8_t **cur,
		const nanotorrent_peer_close_t *message);

void nanotorrent_unpack_peer_close(const uint8_t **cur,
		nanotorrent_peer_close_t *message);

void nanotorrent_pack_peer_have(uint8_t **cur,
		const nanotorrent_peer_have_t *message);

void nanotorrent_unpack_peer_have(const uint8_t **cur,
		nanotorrent_peer_have_t *message);

void nanotorrent_pack_peer_data(uint8_t **cur,
		const nanotorrent_peer_data_t *message);

void nanotorrent_unpack_peer_data(const uint8_t **cur,
		nanotorrent_peer_data_t *message);

#endif /* NANOTORRENT_PACK_PEER_H_ */
