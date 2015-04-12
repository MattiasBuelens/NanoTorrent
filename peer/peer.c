/**
 * \file
 *         Peer communication.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "peer.h"
#include "piece.h"
#include "pack.h"

void nanotorrent_peer_init(nanotorrent_torrent_state_t *state) {

}

void nanotorrent_peer_shutdown(nanotorrent_torrent_state_t *state) {

}

CC_INLINE void nanotorrent_peer_header(const nanotorrent_torrent_state_t *state,
		nanotorrent_peer_message_header_t *header, uint8_t type) {
	sha1_copy(&header->info_hash, &state->info_hash);
	header->type = type;
}

void nanotorrent_peer_write_have(const nanotorrent_torrent_state_t *state,
		uint8_t **data) {
	nanotorrent_peer_have_t message;
	nanotorrent_peer_header(state, &message.header, NANOTRACKER_PEER_HAVE);
	message.have = state->piece.have;

	nanotorrent_pack_peer_have(data, &message);
}

void nanotorrent_peer_data_header(const nanotorrent_torrent_state_t *state,
		uint8_t **buffer, uint8_t type, uint8_t piece_index,
		uint16_t data_start, uint16_t data_length) {
	nanotorrent_peer_data_header_t header;
	nanotorrent_peer_header(state, &header.header, type);
	header.piece_index = piece_index;
	header.data_start = data_start;
	header.data_length = data_length;

	nanotorrent_pack_peer_data_header(buffer, &header);
}

void nanotorrent_peer_write_data_request(
		const nanotorrent_torrent_state_t *state, uint8_t **buffer,
		uint8_t piece_index, uint16_t data_start, uint16_t data_length) {
	nanotorrent_peer_data_header(state, buffer, NANOTRACKER_PEER_DATA_REQUEST,
			piece_index, data_start, data_length);
}

uint16_t nanotorrent_peer_write_data_reply(
		const nanotorrent_torrent_state_t *state, uint8_t **buffer,
		uint16_t buffer_size, uint8_t piece_index, uint16_t data_start) {
	// Write initial header with zero data length
	uint8_t *header_start = *buffer;
	nanotorrent_peer_data_header(state, buffer, NANOTRACKER_PEER_DATA_REPLY,
			piece_index, data_start, 0);
	uint8_t *header_end = *buffer;
	uint16_t header_length = header_end - header_start;

	// Write data
	uint16_t data_length = nanotorrent_piece_read(state, piece_index,
			data_start, *buffer, buffer_size - header_length);
	if (data_length < 0) {
		return -1;
	}
	*buffer += data_length;

	// Write actual data length
	uint8_t *data_length_ptr = header_end - sizeof(data_length);
	nanotorrent_pack_uint16(&data_length_ptr, &data_length);

	return data_length;
}
