/**
 * \file
 *         Piece storage and management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "piece.h"
#include "cfs/cfs.h"

// TODO: Better way to find if Coffee is actually used?
#ifdef __AVR__
#include "cfs/cfs-coffee.h"
#define CFS_RESERVE(name, size) (cfs_coffee_reserve(name, size))
#else
#define CFS_RESERVE(...) (0)
#endif

uint16_t nanotorrent_piece_offset(nanotorrent_torrent_desc_t *desc,
		uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= desc->num_pieces) {
		ERROR("Invalid piece index: %d", piece_index);
		return -1;
	}
	return piece_index * desc->piece_size;
}

uint16_t nanotorrent_piece_size(nanotorrent_torrent_desc_t *desc,
		uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= desc->num_pieces) {
		ERROR("Invalid piece index: %d", piece_index);
		return 0;
	}
	if (piece_index == desc->num_pieces - 1) {
		// Size of last piece may be less than piece size
		uint16_t remainder = desc->file_size % desc->piece_size;
		return remainder == 0 ? desc->piece_size : remainder;
	}
	return desc->piece_size;
}

void nanotorrent_piece_init(nanotorrent_torrent_state_t *state) {
	// Reserve space for file
	int result = CFS_RESERVE(state->file_name, state->desc.file_size);
	if (result == -1) {
		ERROR("Could not reserve %d bytes for file", state->desc.file_size);
		return;
	}
	// Open file
	int file = cfs_open(state->file_name, CFS_READ | CFS_WRITE);
	if (file < 0) {
		ERROR("Could not open file");
		return;
	}
	state->piece.file = file;
	// TODO Initialize completed bit vector
}
