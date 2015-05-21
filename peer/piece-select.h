/**
 * \file
 *         Piece selection.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PIECE_SELECT_H_
#define NANOTORRENT_PIECE_SELECT_H_

#include "common.h"
#include "peer.h"

void nanotorrent_select_init();
bool nanotorrent_select_is_endgame();
bool nanotorrent_select_next(nanotorrent_peer_conn_t *conn,
		uint8_t *piece_index);
void nanotorrent_select_update_have(uint32_t old_have, uint32_t new_have);

#endif /* NANOTORRENT_SELECT_H_ */
