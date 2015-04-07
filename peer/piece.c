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

uint16_t nanotorrent_piece_offset(nanotorrent_torrent_info_t *info,
		uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= info->num_pieces) {
		ERROR("Invalid piece index: %d", piece_index);
		return -1;
	}
	return piece_index * info->piece_size;
}

uint16_t nanotorrent_piece_size(nanotorrent_torrent_info_t *info,
		uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= info->num_pieces) {
		ERROR("Invalid piece index: %d", piece_index);
		return 0;
	}
	if (piece_index == info->num_pieces - 1) {
		// Size of last piece may be less than piece size
		uint16_t remainder = info->file_size % info->piece_size;
		return remainder == 0 ? info->piece_size : remainder;
	}
	return info->piece_size;
}

void nanotorrent_piece_init(nanotorrent_torrent_state_t *state) {
	// Reserve space for file
	int result = CFS_RESERVE(state->file_name, state->desc.info.file_size);
	if (result == -1) {
		ERROR("Could not reserve %d bytes for file",
				state->desc.info.file_size);
		return;
	}
	// Open file
	int file = cfs_open(state->file_name, CFS_READ | CFS_WRITE | CFS_APPEND);
	if (file < 0) {
		ERROR("Could not open file");
		return;
	}
	state->piece.file = file;
	// Verify file contents
	state->piece.completed = nanotorrent_piece_verify_all(state);
}

void nanotorrent_piece_shutdown(nanotorrent_torrent_state_t *state) {
	// Close file
	if (state->piece.file >= 0) {
		cfs_close(state->piece.file);
	}
	state->piece.file = -1;
}

bool nanotorrent_piece_is_complete(nanotorrent_torrent_state_t *state,
		uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= state->desc.info.num_pieces) {
		ERROR("Invalid piece index: %d", piece_index);
		return -1;
	}
	return (state->piece.completed >> piece_index) & 1;
}

bool nanotorrent_piece_verify(nanotorrent_torrent_state_t *state,
		uint8_t piece_index) {
	uint16_t offset = nanotorrent_piece_offset(&state->desc.info, piece_index);
	if (offset < 0) {
		return false;
	}
	// Seek to start of piece
	int file = state->piece.file;
	if (cfs_seek(file, offset, CFS_SEEK_SET) < 0) {
		return false;
	}
	// Prepare digest
	sha1_context_t context;
	sha1_init(&context);
	// Read and process piece
	uint16_t size = nanotorrent_piece_size(&state->desc.info, piece_index);
	uint16_t remaining = size;
	uint8_t buffer[64];
	while (remaining > 0) {
		int read = cfs_read(file, buffer, MIN(remaining, sizeof(buffer)));
		if (read == 0) {
			// File too small
			return false;
		} else if (read < 0) {
			ERROR("Could not read piece %d", piece_index);
			return false;
		}
		if (!sha1_add(&context, buffer, read)) {
			ERROR("Could not digest piece %d", piece_index);
			return false;
		}
		remaining -= read;
	}
	// Calculate digest
	sha1_digest_t digest;
	if (!sha1_result(&context, &digest)) {
		ERROR("Could not calculate digest of piece %d", piece_index);
		return false;
	}
	// Compare calculated hash with expected hash
	return sha1_cmp(&digest, &state->desc.info.piece_hashes[piece_index]);
}

uint32_t nanotorrent_piece_verify_all(nanotorrent_torrent_state_t *state) {
	uint32_t result = 0;
	uint8_t i;
	for (i = 0; i < state->desc.info.num_pieces; i++) {
		bool piece_result = nanotorrent_piece_verify(state, i);
		result |= piece_result << i;
	}
	return result;
}
