/**
 * \file
 *         Packing/unpacking of peer messages.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-peer.h"
#include "pack-basic.h"

void nanotorrent_pack_peer_message_header(uint8_t **cur,
		const nanotorrent_peer_message_header_t *header) {
	nanotorrent_pack_uint8(cur, &header->type);
	nanotorrent_pack_sha1_digest(cur, &header->info_hash);
}

void nanotorrent_unpack_peer_message_header(const uint8_t **cur,
		nanotorrent_peer_message_header_t *header) {
	nanotorrent_unpack_uint8(cur, &header->type);
	nanotorrent_unpack_sha1_digest(cur, &header->info_hash);
}

void nanotorrent_pack_peer_close(uint8_t **cur,
		const nanotorrent_peer_close_t *message) {
	nanotorrent_pack_peer_message_header(cur, &message->header);
}

void nanotorrent_unpack_peer_close(const uint8_t **cur,
		nanotorrent_peer_close_t *message) {
	nanotorrent_unpack_peer_message_header(cur, &message->header);
}

void nanotorrent_pack_peer_have(uint8_t **cur,
		const nanotorrent_peer_have_t *message) {
	nanotorrent_pack_peer_message_header(cur, &message->header);
	nanotorrent_pack_uint32(cur, &message->have);
}

void nanotorrent_unpack_peer_have(const uint8_t **cur,
		nanotorrent_peer_have_t *message) {
	nanotorrent_unpack_peer_message_header(cur, &message->header);
	nanotorrent_unpack_uint32(cur, &message->have);
}

void nanotorrent_pack_peer_data_request(uint8_t **cur,
		const nanotorrent_peer_data_request_t *message) {
	nanotorrent_pack_peer_message_header(cur, &message->header);
	nanotorrent_pack_uint8(cur, &message->piece_index);
	nanotorrent_pack_uint16(cur, &message->data_start);
}

void nanotorrent_unpack_peer_data_request(const uint8_t **cur,
		nanotorrent_peer_data_request_t *message) {
	nanotorrent_unpack_peer_message_header(cur, &message->header);
	nanotorrent_unpack_uint8(cur, &message->piece_index);
	nanotorrent_unpack_uint16(cur, &message->data_start);
}

void nanotorrent_pack_peer_data_reply(uint8_t **cur,
		const nanotorrent_peer_data_reply_t *message) {
	nanotorrent_pack_peer_message_header(cur, &message->header);
	nanotorrent_pack_uint8(cur, &message->piece_index);
	nanotorrent_pack_uint16(cur, &message->data_start);
	nanotorrent_pack_uint16(cur, &message->data_length);
}

void nanotorrent_unpack_peer_data_reply(const uint8_t **cur,
		nanotorrent_peer_data_reply_t *message) {
	nanotorrent_unpack_peer_message_header(cur, &message->header);
	nanotorrent_unpack_uint8(cur, &message->piece_index);
	nanotorrent_unpack_uint16(cur, &message->data_start);
	nanotorrent_unpack_uint16(cur, &message->data_length);
}

