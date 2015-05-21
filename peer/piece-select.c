/**
 * \file
 *         Piece selection.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "piece-select.h"

#include "peer.h"
#include "piece.h"
#include "state.h"
#include "bitset.h"

#define state (nanotorrent_state)

// Amount of occurrences of all pieces at connected peers
static uint8_t piece_counts[NANOTORRENT_MAX_PIECES];

void nanotorrent_select_init() {
	memset(piece_counts, 0, sizeof(piece_counts));
}

void nanotorrent_select_update_have(uint32_t old_have, uint32_t new_have) {
	if (old_have == new_have) {
		return;
	}

	size_t i;
	for (i = 0; i < state.desc.info.num_pieces; i++) {
		bool had_piece = nanotorrent_bitset_get(old_have, i);
		bool has_piece = nanotorrent_bitset_get(new_have, i);
		if (had_piece != has_piece) {
			if (has_piece) {
				piece_counts[i]++;
			} else {
				piece_counts[i]--;
			}
		}
	}
}

bool nanotorrent_select_is_endgame() {
	return nanotorrent_piece_count_complete()
			>= (NANOTORRENT_END_GAME_MIN_COMPLETION * state.desc.info.num_pieces);
}

bool nanotorrent_select_next(nanotorrent_peer_conn_t *conn,
		uint8_t *piece_index) {
	// Get interesting pieces at this peer
	uint32_t interesting = nanotorrent_peer_interesting(conn);
	if (interesting == 0) {
		// All missing pieces already being requested
		if (!nanotorrent_select_is_endgame()) {
			// Too soon to allow end-game mode
			return false;
		}

		// End-game mode: request already requested pieces
		interesting = nanotorrent_peer_interesting_endgame(conn);
		if (interesting == 0) {
			// Still nothing interesting
			return false;
		}
	}

	// Get the rarest interesting piece
	size_t rarest_index = 0;
	uint8_t rarest_count = UINT8_MAX;
	size_t i;
	for (i = 0; i < state.desc.info.num_pieces; i++) {
		if (nanotorrent_bitset_get(interesting, i)
				&& piece_counts[i] < rarest_count) {
			rarest_index = i;
			rarest_count = piece_counts[i];
		}
	}

	*piece_index = rarest_index;
	return true;
}
