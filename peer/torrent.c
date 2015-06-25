/**
 * \file
 *         Torrent management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "torrent.h"
#include "piece.h"
#include "pack.h"
#include "cfs/cfs.h"

bool nanotorrent_torrent_info_hash(const nanotorrent_torrent_info_t *info,
		sha1_digest_t *info_hash) {
	// Pack torrent info
	uint8_t data[sizeof(*info)];
	uint8_t *end = nanotorrent_pack_torrent_info(data, info);
	size_t len = end - data;

	// Calculate hash
	return sha1_compute(data, len, info_hash);
}

bool nanotorrent_torrent_info_create(nanotorrent_torrent_info_t *info,
		const int file, const uint16_t piece_size) {
	// Set file size
	info->file_size = cfs_seek(file, 0l, CFS_SEEK_END);
	cfs_seek(file, 0l, CFS_SEEK_SET);
	PRINTF("File size: %u\n", info->file_size);

	// Set piece size
	info->piece_size = piece_size;

	// Calculate piece hashes
	sha1_context_t context;
	uint8_t piece_index = 0;
	int32_t piece_length = 0;
	do {
		// Digest piece
		sha1_init(&context);
		piece_length = nanotorrent_piece_digest(&context, file, piece_size);
		if (piece_length < 0) {
			// Error
			return false;
		}
		// Only allow empty piece for first piece
		if (piece_index == 0 || piece_length > 0) {
			PRINTF("Piece %u: %u\n", piece_index, piece_length);
			// Calculate and store digest
			if (!sha1_result(&context, &info->piece_hashes[piece_index])) {
				ERROR("Could not calculate digest of piece %u", piece_index);
				return false;
			}
			piece_index++;
		}
	} while (piece_index < NANOTORRENT_MAX_PIECES && piece_length != 0);

	// Make sure we reached the end of the file
	if (piece_length != 0) {
		ERROR("Too many pieces. Try a piece size larger than %u B?",
				piece_size);
		return false;
	}

	// Set number of pieces
	info->num_pieces = piece_index;

	return true;
}
