/**
 * \file
 *         Piece storage and management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PIECE_H_
#define NANOTORRENT_PIECE_H_

#include "common.h"
#include "torrent.h"
#include "state.h"

uint16_t nanotorrent_piece_offset(const nanotorrent_torrent_info_t *info,
		const uint8_t piece_index);
uint16_t nanotorrent_piece_size(const nanotorrent_torrent_info_t *info,
		const uint8_t piece_index);

void nanotorrent_piece_init(nanotorrent_torrent_state_t *state);
void nanotorrent_piece_shutdown(nanotorrent_torrent_state_t *state);
bool nanotorrent_piece_is_complete(const nanotorrent_torrent_state_t *state,
		const uint8_t piece_index);
void nanotorrent_piece_set_complete(const nanotorrent_torrent_state_t *state,
		const uint8_t piece_index, bool is_complete);

uint16_t nanotorrent_piece_read(const nanotorrent_torrent_state_t *state,
		uint8_t piece_index, uint8_t data_offset, uint8_t *buffer,
		uint16_t buffer_length);
uint16_t nanotorrent_piece_write(const nanotorrent_torrent_state_t *state,
		uint8_t piece_index, uint8_t data_offset, const uint8_t *buffer,
		uint16_t buffer_length);

uint16_t nanotorrent_piece_digest(sha1_context_t *context, const int file,
		const uint16_t piece_size);

bool nanotorrent_piece_verify(const nanotorrent_torrent_state_t *state,
		sha1_context_t *context, const uint8_t piece_index);
uint32_t nanotorrent_piece_verify_all(const nanotorrent_torrent_state_t *state,
		sha1_context_t *context);

#endif /* NANOTORRENT_PIECE_H_ */
