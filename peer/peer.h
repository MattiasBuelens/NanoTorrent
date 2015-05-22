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
 * Peer connection
 */
typedef struct nanotorrent_peer_conn nanotorrent_peer_conn_t;

struct nanotorrent_peer_conn {
	/**
	 * Next connection in list
	 */
	nanotorrent_peer_conn_t *next;
	/**
	 * Peer info
	 */
	nanotorrent_peer_info_t peer_info;
	/**
	 * Bit vector of completed pieces in peer's file
	 */
	uint32_t have;
	/**
	 * Heartbeat timer
	 */
	struct etimer heartbeat;
	/**
	 * Whether currently requesting a piece from this peer
	 */
	bool has_request;
	/**
	 * Piece index of requested piece
	 */
	uint8_t request_index;
	/**
	 * Offset in requested piece
	 */
	uint16_t request_offset;
	/**
	 * Request retry
	 */
	nanotorrent_retry_t request_retry;
};

/**
 * Peer message types
 */
enum nanotorrent_peer_message_type {
	/**
	 * Peer closes the connection
	 */
	NANOTORRENT_PEER_CLOSE = 0,
	/**
	 * Peer announces its 'have' state
	 */
	NANOTORRENT_PEER_HAVE,
	/**
	 * Peer requests piece data
	 */
	NANOTORRENT_PEER_DATA_REQUEST,
	/**
	 * Peer sends piece data
	 */
	NANOTORRENT_PEER_DATA_REPLY
};
typedef uint8_t nanotorrent_peer_message_type_t;

typedef struct nanotorrent_peer_message_header {
	/**
	 * Peer message type
	 */
	nanotorrent_peer_message_type_t type;
	/**
	 * SHA1 hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Bit vector of completed pieces in own file
	 */
	uint32_t have;
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
} nanotorrent_peer_have_t;

typedef struct nanotorrent_peer_data {
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
} nanotorrent_peer_data_t;

PROCESS_NAME(nanotorrent_peer_process);
extern process_event_t nanotorrent_peer_event;

#define nanotorrent_peer_is_event(ev) \
	((ev) == nanotorrent_peer_event)

void nanotorrent_peer_start();
void nanotorrent_peer_stop();

uint8_t nanotorrent_peer_count();
nanotorrent_peer_conn_t *nanotorrent_peer_connect(
		const nanotorrent_peer_info_t *peer);
nanotorrent_peer_conn_t *nanotorrent_peer_accept(
		const nanotorrent_peer_info_t *peer);
bool nanotorrent_peer_disconnect(const nanotorrent_peer_info_t *peer);

uint32_t nanotorrent_peer_interesting(nanotorrent_peer_conn_t *conn);
uint32_t nanotorrent_peer_interesting_endgame(nanotorrent_peer_conn_t *conn);

#endif /* NANOTORRENT_PEER_H_ */
