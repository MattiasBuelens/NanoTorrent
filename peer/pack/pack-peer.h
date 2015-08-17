/**
 * \file
 *         Packing/unpacking of peer messages.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_PEER_H_
#define NANOTORRENT_PACK_PEER_H_

#include "../peer.h"

uint8_t *nanotorrent_pack_peer_message_header(uint8_t *buf,
		const nanotorrent_peer_message_header_t *header);

const uint8_t *nanotorrent_unpack_peer_message_header(const uint8_t *buf,
		nanotorrent_peer_message_header_t *header);

uint8_t *nanotorrent_pack_peer_close(uint8_t *buf,
		const nanotorrent_peer_close_t *message);

const uint8_t *nanotorrent_unpack_peer_close(const uint8_t *buf,
		nanotorrent_peer_close_t *message);

uint8_t *nanotorrent_pack_peer_have(uint8_t *buf,
		const nanotorrent_peer_have_t *message);

const uint8_t *nanotorrent_unpack_peer_have(const uint8_t *buf,
		nanotorrent_peer_have_t *message);

uint8_t *nanotorrent_pack_peer_data(uint8_t *buf,
		const nanotorrent_peer_data_t *message);

const uint8_t *nanotorrent_unpack_peer_data(const uint8_t *buf,
		nanotorrent_peer_data_t *message);

#endif /* NANOTORRENT_PACK_PEER_H_ */
