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

uint16_t nanotorrent_piece_offset(nanotorrent_torrent_desc_t *desc,
		uint8_t piece_index);
uint16_t nanotorrent_piece_size(nanotorrent_torrent_desc_t *desc,
		uint8_t piece_index);

void nanotorrent_piece_init(nanotorrent_torrent_state_t *state);
int nanotorrent_piece_is_complete(nanotorrent_torrent_state_t *state,
		uint8_t piece_index);
int nanotorrent_piece_verify(nanotorrent_torrent_state_t *state,
		uint8_t piece_index);
int nanotorrent_piece_verify_all(nanotorrent_torrent_state_t *state);

#endif /* NANOTORRENT_PIECE_H_ */
