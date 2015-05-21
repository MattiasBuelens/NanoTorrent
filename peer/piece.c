/**
 * \file
 *         Piece storage and management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "piece.h"
#include "bitset.h"
#include "cfs/cfs.h"

#define state (nanotorrent_state)

#define NANOTORRENT_PIECE_BUFFER_SIZE 256

/**
 * File descriptor for own file
 */
static int own_file;
/**
 * Bit vector of completed pieces in own file
 */
static uint32_t own_have;

uint16_t nanotorrent_piece_offset(const nanotorrent_torrent_info_t *info,
		const uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= info->num_pieces) {
		ERROR("Invalid piece index: %u", piece_index);
		return -1;
	}
	return piece_index * info->piece_size;
}

uint16_t nanotorrent_piece_size(const nanotorrent_torrent_info_t *info,
		const uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= info->num_pieces) {
		ERROR("Invalid piece index: %u", piece_index);
		return -1;
	}
	if (piece_index == info->num_pieces - 1) {
		// Size of last piece may be less than piece size
		if (piece_index == 0) {
			// Single piece holding whole file
			return info->file_size;
		} else {
			// Multiple pieces, ensure no piece has zero size
			uint16_t remainder = info->file_size % info->piece_size;
			return remainder == 0 ? info->piece_size : remainder;
		}
	}
	return info->piece_size;
}

void nanotorrent_piece_init() {
	// Open file
	int file = cfs_open(state.file_name, CFS_READ | CFS_WRITE | CFS_APPEND);
	if (file < 0) {
		ERROR("Could not open file");
		return;
	}
	own_file = file;
	// Verify file contents
	sha1_context_t context;
	own_have = nanotorrent_piece_verify_all(&context);
}

void nanotorrent_piece_shutdown() {
	// Close file
	if (own_file >= 0) {
		cfs_close(own_file);
	}
	own_file = -1;
}

uint32_t nanotorrent_piece_bitset_all() {
	return (1 << state.desc.info.num_pieces) - 1;
}

uint32_t nanotorrent_piece_have() {
	return own_have;
}

bool nanotorrent_piece_is_seed() {
	uint32_t all = nanotorrent_piece_bitset_all();
	return nanotorrent_bitset_contains(all, own_have);
}

bool nanotorrent_piece_is_complete(const uint8_t piece_index) {
	if (piece_index < 0 || piece_index >= state.desc.info.num_pieces) {
		ERROR("Invalid piece index: %u", piece_index);
		return false;
	}
	return nanotorrent_bitset_get(own_have, piece_index);
}

void nanotorrent_piece_set_complete(const uint8_t piece_index, bool is_complete) {
	if (piece_index < 0 || piece_index >= state.desc.info.num_pieces) {
		ERROR("Invalid piece index: %u", piece_index);
		return;
	}
	if (is_complete) {
		nanotorrent_bitset_set(own_have, piece_index);
	} else {
		nanotorrent_bitset_clear(own_have, piece_index);
	}
}

uint8_t nanotorrent_piece_count_complete() {
	return nanotorrent_bitset_count(own_have);
}

uint16_t nanotorrent_piece_read(const uint8_t piece_index,
		const uint8_t data_offset, uint8_t *buffer,
		const uint16_t buffer_length) {
	uint16_t piece_size = nanotorrent_piece_size(&state.desc.info, piece_index);
	if (piece_size < 0) {
		ERROR("Invalid piece index: %u", piece_index);
		return -1;
	}
	if (data_offset >= piece_size) {
		ERROR("Data offset %u exceeds piece size %u for piece index %u",
				data_offset, piece_size, piece_index);
		return -1;
	}
	// Seek to start of requested piece data
	uint16_t piece_offset = nanotorrent_piece_offset(&state.desc.info,
			piece_index);
	uint16_t offset = piece_offset + data_offset;
	if (cfs_seek(own_file, offset, CFS_SEEK_SET) < 0) {
		ERROR("Could not seek to piece %u at offset %u", piece_index, offset);
		return -1;
	}
	// Read piece data into buffer
	uint16_t data_length = piece_size - data_offset;
	uint16_t read = cfs_read(own_file, buffer, MIN(buffer_length, data_length));
	if (read < 0) {
		ERROR("Could not read piece %u at offset %u", piece_index, offset);
		return -1;
	}
	return read;
}

uint16_t nanotorrent_piece_write(const uint8_t piece_index,
		const uint8_t data_offset, const uint8_t *buffer,
		const uint16_t buffer_length) {
	uint16_t piece_size = nanotorrent_piece_size(&state.desc.info, piece_index);
	if (piece_size < 0) {
		ERROR("Invalid piece index: %u", piece_index);
		return -1;
	}
	if (data_offset >= piece_size) {
		ERROR("Data offset %u exceeds piece size %u for piece index %u",
				data_offset, piece_size, piece_index);
		return -1;
	}
	// Seek to start of provided piece data
	uint16_t piece_offset = nanotorrent_piece_offset(&state.desc.info,
			piece_index);
	uint16_t offset = piece_offset + data_offset;
	if (cfs_seek(own_file, offset, CFS_SEEK_SET) < 0) {
		ERROR("Could not seek to piece %u at offset %u", piece_index, offset);
		return -1;
	}
	// Write buffer into piece data
	uint16_t data_length = piece_size - data_offset;
	uint16_t written = cfs_write(own_file, buffer,
			MIN(buffer_length, data_length));
	if (written < 0) {
		ERROR("Could not write piece %u at offset %u", piece_index, offset);
		return -1;
	}
	return written;
}

uint16_t nanotorrent_piece_digest(sha1_context_t *context, const int file,
		const uint16_t piece_size) {
	// Process at most piece_size bytes
	uint16_t length = 0;
	uint16_t remaining = piece_size;
	uint8_t buffer[NANOTORRENT_PIECE_BUFFER_SIZE];
	while (remaining > 0) {
		// Read into buffer
		int read = cfs_read(file, buffer, MIN(remaining, sizeof(buffer)));
		if (read == 0) {
			// At EOF
			break;
		} else if (read < 0) {
			ERROR("Could not read piece");
			return -1;
		}
		// Process from buffer
		if (!sha1_add(context, buffer, read)) {
			ERROR("Could not digest piece");
			return -1;
		}
		length += read;
		remaining -= read;
	}
	// Return number of bytes read
	return length;
}

bool nanotorrent_piece_verify(sha1_context_t *context,
		const uint8_t piece_index) {
	uint16_t offset = nanotorrent_piece_offset(&state.desc.info, piece_index);
	if (offset < 0) {
		return false;
	}
	uint16_t size = nanotorrent_piece_size(&state.desc.info, piece_index);
	// Seek to start of piece
	int file = own_file;
	if (cfs_seek(file, offset, CFS_SEEK_SET) < 0) {
		return false;
	}
	// Calculate piece digest
	sha1_digest_t digest;
	sha1_init(context);
	uint16_t piece_length = nanotorrent_piece_digest(context, file, size);
	if (piece_length < 0) {
		return false;
	}
	if (!sha1_result(context, &digest)) {
		ERROR("Could not calculate digest of piece %u", piece_index);
		return false;
	}
	// Compare calculated hash with expected hash
	return sha1_cmp(&digest, &state.desc.info.piece_hashes[piece_index]);
}

uint32_t nanotorrent_piece_verify_all(sha1_context_t *context) {
	uint32_t result = 0;
	uint8_t i;
	for (i = 0; i < state.desc.info.num_pieces; i++) {
		if (nanotorrent_piece_verify(context, i)) {
			nanotorrent_bitset_set(result, i);
		}
	}
	return result;
}
