/**
 * \file
 *         Peer communication.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "peer.h"
#include "piece.h"
#include "piece-select.h"
#include "pack.h"
#include "bitset.h"

#include "lib/list.h"
#include "lib/memb.h"

#define state (nanotorrent_state)

/**
 * Peer connections
 */
LIST(peers);
MEMB(peers_in, nanotorrent_peer_conn_t, NANOTORRENT_MAX_IN_PEERS);
MEMB(peers_out, nanotorrent_peer_conn_t, NANOTORRENT_MAX_OUT_PEERS);

/**
 * Bitset of pending pieces
 */
static uint32_t pending_pieces;

/**
 * UDP socket with peers
 */
static struct udp_socket peer_socket;

/**
 * Periodic heartbeat timer
 */
static struct etimer heartbeat;

void nanotorrent_peer_send_close(const nanotorrent_peer_info_t *peer);
void nanotorrent_peer_send_data_request(const nanotorrent_peer_info_t *peer,
		uint8_t piece_index, uint16_t data_start);

void nanotorrent_peer_handle_request_retry(nanotorrent_retry_event_t event,
		void *data);

void nanotorrent_peer_handle_message(struct udp_socket *peer_socket, void *ptr,
		const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen);

void nanotorrent_peer_start() {
	process_start(&nanotorrent_peer_process, NULL);
}

void nanotorrent_peer_stop() {
	process_exit(&nanotorrent_peer_process);
}

void nanotorrent_peer_init() {
	// Initialize peers
	list_init(peers);
	memb_init(&peers_in);
	memb_init(&peers_out);
	// Start heartbeat
	etimer_set(&heartbeat, NANOTORRENT_PEER_HEARTBEAT_PERIOD);
	// Register peer socket
	udp_socket_close(&peer_socket);
	udp_socket_register(&peer_socket, NULL, nanotorrent_peer_handle_message);
	udp_socket_bind(&peer_socket, NANOTORRENT_PEER_PORT);
	PRINTF("Listening for peers on port %u\n", NANOTORRENT_PEER_PORT);
}

void nanotorrent_peer_shutdown() {
	// Close peer socket
	udp_socket_close(&peer_socket);
	// Stop heartbeat
	etimer_stop(&heartbeat);
}

uint8_t nanotorrent_peer_count() {
	return list_length(peers);
}

nanotorrent_peer_conn_t *nanotorrent_peer_find(
		const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_conn_t *conn;
	for (conn = list_head(peers); conn != NULL; conn = list_item_next(conn)) {
		if (nanotorrent_peer_info_cmp(peer, &conn->peer_info)) {
			return conn;
		}
	}
	return NULL;
}

nanotorrent_peer_conn_t *nanotorrent_peer_allocate_out() {
	return memb_alloc(&peers_out);
}

nanotorrent_peer_conn_t *nanotorrent_peer_allocate_in() {
	return memb_alloc(&peers_in);
}

void nanotorrent_peer_free(nanotorrent_peer_conn_t *conn) {
	if (memb_inmemb(&peers_in, conn)) {
		memb_free(&peers_in, conn);
	} else if (memb_inmemb(&peers_out, conn)) {
		memb_free(&peers_out, conn);
	}
}

void nanotorrent_peer_add(nanotorrent_peer_conn_t *conn) {
	// Ensure timers are bound to peer process
	PROCESS_CONTEXT_BEGIN(&nanotorrent_peer_process);

	// Initialize
	conn->have = 0;
	conn->has_request = false;
	nanotorrent_retry_init(&conn->request_retry,
			(NANOTORRENT_PEER_RETRY_TIMEOUT),
			nanotorrent_peer_handle_request_retry);
	// Start heartbeat
	etimer_set(&conn->heartbeat, NANOTORRENT_PEER_HEARTBEAT_TIMEOUT);
	// Add to peers list
	list_push(peers, conn);

	PROCESS_CONTEXT_END(&nanotorrent_peer_process);
}

void nanotorrent_peer_remove(nanotorrent_peer_conn_t *conn) {
	// Remove from peers list
	list_remove(peers, conn);
	// Remove from piece counts
	nanotorrent_select_update_have(conn->have, 0);
	conn->have = 0;
	conn->has_request = false;
	// Stop heartbeat
	etimer_stop(&conn->heartbeat);
}

nanotorrent_peer_conn_t *nanotorrent_peer_connect_with(
		const nanotorrent_peer_info_t *peer, struct memb *pool) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer);
	if (conn != NULL) {
		// Already connected with this peer
		return conn;
	}
	// Find available slot
	conn = memb_alloc(pool);
	if (conn == NULL) {
		// No more slots available
		return NULL;
	}
	// Add peer connection
	conn->peer_info = *peer;
	nanotorrent_peer_add(conn);
	// TODO Notify connect
	return conn;
}

nanotorrent_peer_conn_t *nanotorrent_peer_connect(
		const nanotorrent_peer_info_t *peer) {
	return nanotorrent_peer_connect_with(peer, &peers_out);
}

void nanotorrent_peer_connect_all(const nanotorrent_peer_info_t *peers,
		uint8_t num_peers) {
	int i;
	for (i = 0; i < num_peers; i++) {
		if (nanotorrent_peer_connect(&peers[i]) == NULL) {
			// No more slots available
			break;
		}
	}
}

nanotorrent_peer_conn_t *nanotorrent_peer_accept(
		const nanotorrent_peer_info_t *peer) {
	return nanotorrent_peer_connect_with(peer, &peers_in);
}

void nanotorrent_peer_force_disconnect_conn(nanotorrent_peer_conn_t *conn) {
	nanotorrent_peer_remove(conn);
	nanotorrent_peer_free(conn);
	// TODO Notify disconnect
}

void nanotorrent_peer_disconnect_conn(nanotorrent_peer_conn_t *conn) {
	// Disconnect locally
	nanotorrent_peer_force_disconnect_conn(conn);
	// Send CLOSE message
	nanotorrent_peer_send_close(&conn->peer_info);
}

bool nanotorrent_peer_force_disconnect(const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer);
	if (conn == NULL) {
		return false;
	}
	nanotorrent_peer_force_disconnect_conn(conn);
	return true;
}

bool nanotorrent_peer_disconnect(const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer);
	if (conn == NULL) {
		return false;
	}
	nanotorrent_peer_disconnect_conn(conn);
	return true;
}

uint32_t nanotorrent_peer_interesting(nanotorrent_peer_conn_t *conn) {
	uint32_t interesting = conn->have;
	interesting = nanotorrent_bitset_diff(interesting,
			nanotorrent_piece_have());
	interesting = nanotorrent_bitset_diff(interesting, pending_pieces);
	return interesting;
}

uint32_t nanotorrent_peer_interesting_endgame(nanotorrent_peer_conn_t *conn) {
	uint32_t interesting = conn->have;
	interesting = nanotorrent_bitset_diff(interesting,
			nanotorrent_piece_have());
	return interesting;
}

void nanotorrent_peer_update_have(nanotorrent_peer_conn_t *conn, uint32_t have) {
	// Update have field and piece counts
	uint32_t new_have = conn->have | have;
	nanotorrent_select_update_have(conn->have, new_have);
	conn->have = new_have;
	// Heartbeat is still alive
	etimer_restart(&conn->heartbeat);
}

bool nanotorrent_peer_has_request(uint8_t piece_index) {
	return nanotorrent_bitset_get(pending_pieces, piece_index);
}

nanotorrent_peer_conn_t *nanotorrent_peer_find_request(uint8_t piece_index) {
	nanotorrent_peer_conn_t *conn;
	for (conn = list_head(peers); conn != NULL; conn = list_item_next(conn)) {
		if (conn->has_request && conn->request_index == piece_index) {
			return conn;
		}
	}
	return NULL;
}

void nanotorrent_peer_request_init(nanotorrent_peer_conn_t *conn) {
	conn->has_request = true;
	nanotorrent_bitset_set(pending_pieces, conn->request_index);
}

void nanotorrent_peer_request_start(nanotorrent_peer_conn_t *conn) {
	nanotorrent_peer_request_init(conn);
	nanotorrent_retry_start(&conn->request_retry,
			(NANOTORRENT_MAX_PEER_RETRIES), conn);
}

void nanotorrent_peer_request_stop(nanotorrent_peer_conn_t *conn) {
	conn->has_request = false;
	nanotorrent_bitset_clear(pending_pieces, conn->request_index);
	nanotorrent_retry_stop(&conn->request_retry);
}

bool nanotorrent_peer_request_next(nanotorrent_peer_conn_t *conn) {
	if (conn->has_request) {
		// Already requesting
		return false;
	}
	if (nanotorrent_piece_is_seed()) {
		// Seeding, nothing left to request
		return false;
	}
	// Request next piece
	if (!nanotorrent_select_next(conn, &conn->request_index)) {
		return false;
	}
	conn->request_offset = 0;
	nanotorrent_peer_request_start(conn);
	return true;
}

bool nanotorrent_peer_should_receive_data(const nanotorrent_peer_info_t *peer,
		uint8_t piece_index, uint16_t data_offset, uint16_t data_length) {
	nanotorrent_peer_conn_t *conn;
	// Ignore if we already have piece
	if (nanotorrent_piece_is_complete(piece_index)) {
		return false;
	}
	// Requesting this piece?
	if (nanotorrent_peer_has_request(piece_index)) {
		conn = nanotorrent_peer_find_request(piece_index);
		// Data in range of current request?
		if (data_offset <= conn->request_offset
				&& conn->request_offset < data_offset + data_length) {
			// Data valid for current request
			return true;
		}
	}
#if NANOTORRENT_LOCAL
	else if (data_offset == 0) {
		// Peer started sending data for a new piece to some
		// other peer, but we are not yet requesting that piece.
		// Try to add a request so we can receive the data ourselves
		// instead of having to make a new request later on.
		nanotorrent_peer_conn_t *conn;
		conn = nanotorrent_peer_connect(peer);
		if (conn == NULL) {
			// Cannot connect
			return false;
		}
		if (conn->has_request) {
			// Peer already handling other request
			return false;
		}
		// Create request
		// Will be handled in nanotorrent_peer_data_received
		conn->request_index = piece_index;
		conn->request_offset = data_offset;
		nanotorrent_peer_request_init(conn);
		return true;
	}
#endif /* NANOTORRENT_LOCAL */
	return false;
}

void nanotorrent_peer_data_received(const nanotorrent_peer_info_t *peer,
		uint8_t piece_index, uint16_t data_offset, uint16_t data_length) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find_request(piece_index);
	if (conn == NULL) {
		ERROR("Expected peer request for piece %u", piece_index);
		return;
	}
	// Is piece completed?
	uint16_t piece_size = nanotorrent_piece_size(&state.desc.info, piece_index);
	uint16_t next_offset = data_offset + data_length;
	if (next_offset < piece_size) {
		// Start next request
		conn->request_offset = next_offset;
		nanotorrent_peer_request_start(conn);
	} else {
		// Piece fully received
		nanotorrent_peer_request_stop(conn);
		// Verify piece
		sha1_context_t context;
		bool is_complete = nanotorrent_piece_verify(&context, piece_index);
		nanotorrent_piece_set_complete(piece_index, is_complete);
		if (is_complete) {
			// Piece completed
			NOTE("Piece %u completed", piece_index);
		} else {
			// Piece corrupted
			WARN("Piece %u corrupted", piece_index);
		}
	}
}

void nanotorrent_peer_handle_request_retry(nanotorrent_retry_event_t event,
		void *data) {
	nanotorrent_peer_conn_t *conn = data;
	if (!conn->has_request)
		return;

	switch (event) {
	case RETRY_AGAIN:
		// Try again
		// Send piece request
		nanotorrent_peer_send_data_request(&conn->peer_info,
				conn->request_index, conn->request_offset);
		break;
	case RETRY_STOP:
		// Stopped retrying
		// Cancel request
		nanotorrent_peer_request_stop(conn);
		// Request next piece
		nanotorrent_peer_request_next(conn);
		break;
	}
}

void nanotorrent_peer_send_message(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *peer) {
	udp_socket_sendto(&peer_socket, buffer, buffer_length, &peer->peer_ip,
			(NANOTORRENT_PEER_PORT));
}

CC_INLINE void nanotorrent_peer_make_header(
		nanotorrent_peer_message_header_t *header, uint8_t type) {
	sha1_copy(&header->info_hash, &state.info_hash);
	header->type = type;
	header->have = nanotorrent_piece_have();
}

void nanotorrent_peer_send_close(const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_close_t message;
	nanotorrent_peer_make_header(&message.header, NANOTORRENT_PEER_CLOSE);

	uint8_t buffer[sizeof(message)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_close(&cur, &message);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, peer);
}

void nanotorrent_peer_send_have(const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_have_t message;
	nanotorrent_peer_make_header(&message.header, NANOTORRENT_PEER_HAVE);

	uint8_t buffer[sizeof(message)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_have(&cur, &message);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, peer);
}

#if NANOTORRENT_LOCAL
bool nanotorrent_peer_is_local(const nanotorrent_peer_info_t *peer) {
	return uip_is_addr_linklocal(&peer->peer_ip);
}

void nanotorrent_peer_send_local_multicast(const uint8_t *buffer,
		uint16_t buffer_length) {
	uip_ip6addr_t dest_addr;
	uip_create_linklocal_allnodes_mcast(&dest_addr);
	udp_socket_sendto(&peer_socket, buffer, buffer_length, &dest_addr,
			(NANOTORRENT_PEER_PORT));
}

void nanotorrent_peer_send_have_local_multicast() {
	nanotorrent_peer_have_t message;
	nanotorrent_peer_make_header(&message.header, NANOTORRENT_PEER_HAVE);

	uint8_t buffer[sizeof(message)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_have(&cur, &message);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_local_multicast(buffer, length);
}
#endif /* NANOTORRENT_LOCAL */

void nanotorrent_peer_send_data_request(const nanotorrent_peer_info_t *peer,
		uint8_t piece_index, uint16_t data_start) {
	nanotorrent_peer_data_t request;
	nanotorrent_peer_make_header(&request.header,
			NANOTORRENT_PEER_DATA_REQUEST);
	request.piece_index = piece_index;
	request.data_start = data_start;

	uint8_t buffer[sizeof(request)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_data(&cur, &request);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, peer);
}

uint16_t nanotorrent_peer_write_data_reply(uint8_t **cur, uint16_t buffer_size,
		uint8_t piece_index, uint16_t data_start) {
	// Write header
	nanotorrent_peer_data_t reply;
	nanotorrent_peer_make_header(&reply.header, NANOTORRENT_PEER_DATA_REPLY);
	reply.piece_index = piece_index;
	reply.data_start = data_start;

	uint8_t *header_start = *cur;
	nanotorrent_pack_peer_data(cur, &reply);
	uint8_t *header_end = *cur;
	uint16_t header_length = header_end - header_start;

	// Write data
	uint16_t data_length = nanotorrent_piece_read(piece_index, data_start, *cur,
			buffer_size - header_length);
	if (data_length < 0) {
		return 0;
	}
	*cur += data_length;

	// Return number of data bytes in reply
	return data_length;
}

void nanotorrent_peer_handle_data_request(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_data_t request;

	// Parse request header
	const uint8_t *cur = buffer;
	nanotorrent_unpack_peer_data(&cur, &request);

	// Check if we have requested piece
	if (!nanotorrent_piece_is_complete(request.piece_index)) {
		// Request was probably broadcasted
		return;
	}

	// Reply with (part of) requested data
	uint8_t reply_buffer[NANOTORRENT_MAX_UDP_PAYLOAD_SIZE];
	uint8_t *reply_cur = reply_buffer;
	uint16_t data_length = nanotorrent_peer_write_data_reply(&reply_cur,
			sizeof(reply_buffer), request.piece_index, request.data_start);
	if (data_length == 0) {
		return;
	}

	// Send reply
	uint16_t reply_length = reply_cur - reply_buffer;

#if NANOTORRENT_LOCAL
	if (nanotorrent_peer_is_local(peer)) {
		nanotorrent_peer_send_local_multicast(reply_buffer, reply_length);
	} else {
		nanotorrent_peer_send_message(reply_buffer, reply_length, peer);
	}
#else
	nanotorrent_peer_send_message(reply_buffer, reply_length, peer);
#endif
}

void nanotorrent_peer_handle_data_reply(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *peer) {
	nanotorrent_peer_data_t reply;

	// Parse reply header
	const uint8_t *cur = buffer;
	nanotorrent_unpack_peer_data(&cur, &reply);
	uint16_t header_length = cur - buffer;
	uint16_t data_length = buffer_length - header_length;

	// Check if we are interested in this data
	if (nanotorrent_peer_should_receive_data(peer, reply.piece_index,
			reply.data_start, data_length)) {
		return;
	}

	// Write piece data
	const uint8_t *data = cur;
	uint16_t written = nanotorrent_piece_write(reply.piece_index,
			reply.data_start, data, data_length);
	if (written < 0) {
		return;
	}

	// Handle receipt
	nanotorrent_peer_data_received(peer, reply.piece_index, reply.data_start,
			written);
}

void nanotorrent_peer_handle_message(struct udp_socket *peer_socket, void *ptr,
		const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen) {
	nanotorrent_peer_message_header_t header;

	// Parse peer message header
	const uint8_t *cur = data;
	nanotorrent_unpack_peer_message_header(&cur, &header);

	// Compare torrent info hash
	if (!sha1_cmp(&state.info_hash, &header.info_hash)) {
		WARN("Ignoring peer message for unknown torrent ");
		sha1_print(&header.info_hash);
		PRINTF("\n");
		return;
	}

	// Remote peer info
	nanotorrent_peer_info_t peer;
	uip_ip6addr_copy(&peer.peer_ip, src_addr);

	if (header.type == NANOTORRENT_PEER_CLOSE) {
		// Handle close immediately
		nanotorrent_peer_force_disconnect(&peer);
		return;
	} else if (header.type == NANOTORRENT_PEER_DATA_REPLY) {
		// Handle data reply first
		// This can add opportunistic connections and requests
		nanotorrent_peer_handle_data_reply(data, datalen, &peer);
	}

	// Accept connection
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_accept(&peer);
	if (conn == NULL) {
		WARN("Cannot accept peer");
		PRINT6ADDR(&peer.peer_ip);
		PRINTF("\n");
		return;
	}

	// Update peer state
	nanotorrent_peer_update_have(conn, header.have);

	switch (header.type) {
	case NANOTORRENT_PEER_HAVE:
	case NANOTORRENT_PEER_DATA_REPLY:
		// Request next piece
		nanotorrent_peer_request_next(conn);
		break;
	case NANOTORRENT_PEER_DATA_REQUEST:
		// Reply to data request
		nanotorrent_peer_handle_data_request(data, datalen, &peer);
		break;
	default:
		WARN("Ignoring peer message with unknown type %u", header.type);
	}
}

PROCESS(nanotorrent_peer_process, "NanoTorrent peer process");
PROCESS_THREAD(nanotorrent_peer_process, ev, data) {
	PROCESS_EXITHANDLER(nanotorrent_peer_shutdown())
	PROCESS_BEGIN()

		// Initialize
		nanotorrent_peer_event = process_alloc_event();
		nanotorrent_peer_init();

		while (true) {
			nanotorrent_peer_conn_t *conn, *next_conn;
			for (conn = list_head(peers); conn != NULL; conn = next_conn) {
				// Get next peer before messing with peers list
				next_conn = list_item_next(conn);
				// Handle dropped heartbeat
				if (etimer_expired(&conn->heartbeat)) {
					nanotorrent_peer_disconnect_conn(conn);
				}
				// Handle request retries
				if (conn->has_request) {
					nanotorrent_retry_process(&conn->request_retry);
				}
				// Send own heartbeat
				if (etimer_expired(&heartbeat)
#if NANOTORRENT_LOCAL
						&& !nanotorrent_peer_is_local(&conn->peer_info)
#endif
						) {
					nanotorrent_peer_send_have(&conn->peer_info);
				}
			}

			if (etimer_expired(&heartbeat)) {
#if NANOTORRENT_LOCAL
				// Send local multicast heartbeat
				nanotorrent_peer_send_have_local_multicast();
#endif
				// Schedule next heartbeat
				etimer_reset(&heartbeat);
			}

			// Wait for timer event
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		}

	PROCESS_END()
}
