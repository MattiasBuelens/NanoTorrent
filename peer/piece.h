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
		uint8_t piece_index);
uint16_t nanotorrent_piece_size(const nanotorrent_torrent_info_t *info,
		uint8_t piece_index);

void nanotorrent_piece_init();
void nanotorrent_piece_shutdown();

uint32_t nanotorrent_piece_bitset_all();

uint32_t nanotorrent_piece_have();
bool nanotorrent_piece_is_seed();
bool nanotorrent_piece_is_complete(uint8_t piece_index);
void nanotorrent_piece_set_complete(uint8_t piece_index, bool is_complete);
uint8_t nanotorrent_piece_count_complete();

uint16_t nanotorrent_piece_read(uint8_t piece_index, uint8_t data_offset,
		uint8_t *buffer, uint16_t buffer_length);
uint16_t nanotorrent_piece_write(uint8_t piece_index, uint8_t data_offset,
		const uint8_t *buffer, uint16_t buffer_length);

uint16_t nanotorrent_piece_digest(sha1_context_t *context, int file,
		uint16_t piece_size);

bool nanotorrent_piece_verify(sha1_context_t *context, uint8_t piece_index);
uint32_t nanotorrent_piece_verify_all(sha1_context_t *context);

#endif /* NANOTORRENT_PIECE_H_ */
