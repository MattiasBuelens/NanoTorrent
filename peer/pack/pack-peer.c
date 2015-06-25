/**
 * \file
 *         Packing/unpacking of peer messages.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-peer.h"
#include "pack-basic.h"

uint8_t *nanotorrent_pack_peer_message_header(uint8_t *buf,
		const nanotorrent_peer_message_header_t *header) {
	buf = nanotorrent_pack_uint8(buf, &header->type);
	buf = nanotorrent_pack_sha1_digest(buf, &header->info_hash);
	buf = nanotorrent_pack_uint32(buf, &header->have);
	return buf;
}

const uint8_t *nanotorrent_unpack_peer_message_header(const uint8_t *buf,
		nanotorrent_peer_message_header_t *header) {
	buf = nanotorrent_unpack_uint8(buf, &header->type);
	buf = nanotorrent_unpack_sha1_digest(buf, &header->info_hash);
	buf = nanotorrent_unpack_uint32(buf, &header->have);
	return buf;
}

uint8_t *nanotorrent_pack_peer_close(uint8_t *buf,
		const nanotorrent_peer_close_t *message) {
	buf = nanotorrent_pack_peer_message_header(buf, &message->header);
	return buf;
}

const uint8_t *nanotorrent_unpack_peer_close(const uint8_t *buf,
		nanotorrent_peer_close_t *message) {
	buf = nanotorrent_unpack_peer_message_header(buf, &message->header);
	return buf;
}

uint8_t *nanotorrent_pack_peer_have(uint8_t *buf,
		const nanotorrent_peer_have_t *message) {
	buf = nanotorrent_pack_peer_message_header(buf, &message->header);
	return buf;
}

const uint8_t *nanotorrent_unpack_peer_have(const uint8_t *buf,
		nanotorrent_peer_have_t *message) {
	buf = nanotorrent_unpack_peer_message_header(buf, &message->header);
	return buf;
}

uint8_t *nanotorrent_pack_peer_data(uint8_t *buf,
		const nanotorrent_peer_data_t *message) {
	buf = nanotorrent_pack_peer_message_header(buf, &message->header);
	buf = nanotorrent_pack_uint8(buf, &message->piece_index);
	buf = nanotorrent_pack_uint16(buf, &message->data_start);
	return buf;
}

const uint8_t *nanotorrent_unpack_peer_data(const uint8_t *buf,
		nanotorrent_peer_data_t *message) {
	buf = nanotorrent_unpack_peer_message_header(buf, &message->header);
	buf = nanotorrent_unpack_uint8(buf, &message->piece_index);
	buf = nanotorrent_unpack_uint16(buf, &message->data_start);
	return buf;
}

