/**
 * \file
 *         Peer communication.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PEER_H_
#define NANOTORRENT_PEER_H_

#include "common.h"
#include "state.h"

/**
 * Peer message types
 */
enum nanotracker_peer_message_type {
	/**
	 * Peer closes the connection
	 */
	NANOTRACKER_PEER_CLOSE = 0,
	/**
	 * Peer announces its 'have' state
	 */
	NANOTRACKER_PEER_HAVE,
	/**
	 * Peer requests piece data
	 */
	NANOTRACKER_PEER_DATA_REQUEST,
	/**
	 * Peer sends piece data
	 */
	NANOTRACKER_PEER_DATA_REPLY
};
typedef uint8_t nanotracker_peer_message_type_t;

typedef struct nanotorrent_peer_message_header {
	/**
	 * Peer message type
	 */
	nanotracker_peer_message_type_t type;
	/**
	 * SHA1 hash of torrent info
	 */
	sha1_digest_t info_hash;
} nanotorrent_peer_message_header_t;

typedef struct nanotorrent_peer_close {
	/**
	 * Message header
	 */
	nanotorrent_peer_message_header_t header;
} nanotorrent_peer_close_t;

typedef struct nanotorrent_peer_have {
	/**
	 * Message header
	 */
	nanotorrent_peer_message_header_t header;
	/**
	 * Bit vector of completed pieces in own file
	 */
	uint32_t have;
} nanotorrent_peer_have_t;

typedef struct nanotorrent_peer_data_request {
	/**
	 * Message header
	 */
	nanotorrent_peer_message_header_t header;
	/**
	 * Requested piece index
	 */
	uint8_t piece_index;
	/**
	 * Data offset from piece start
	 */
	uint16_t data_start;
} nanotorrent_peer_data_request_t;

typedef struct nanotorrent_peer_data_reply {
	/**
	 * Message header
	 */
	nanotorrent_peer_message_header_t header;
	/**
	 * Requested piece index
	 */
	uint8_t piece_index;
	/**
	 * Data offset from piece start
	 */
	uint16_t data_start;
	/**
	 * Data length
	 */
	uint16_t data_length;
} nanotorrent_peer_data_reply_t;

void nanotorrent_peer_init(nanotorrent_torrent_state_t *state);
void nanotorrent_peer_shutdown(nanotorrent_torrent_state_t *state);

void nanotorrent_peer_write_close(const nanotorrent_torrent_state_t *state,
		uint8_t **cur);
void nanotorrent_peer_write_have(const nanotorrent_torrent_state_t *state,
		uint8_t **cur);
void nanotorrent_peer_write_data_request(
		const nanotorrent_torrent_state_t *state, uint8_t **cur,
		uint8_t piece_index, uint16_t data_start);
uint16_t nanotorrent_peer_write_data_reply(
		const nanotorrent_torrent_state_t *state, uint8_t **cur,
		uint16_t buffer_size, uint8_t piece_index, uint16_t data_start);

#endif /* NANOTORRENT_PEER_H_ */
