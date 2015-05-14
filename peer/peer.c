/**
 * \file
 *         Peer communication.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "peer.h"
#include "piece.h"
#include "pack.h"

#define state (nanotorrent_state)

/**
 * UDP socket with peers
 */
static struct udp_socket peer_socket;

void nanotorrent_peer_send_close(const nanotorrent_peer_info_t *remote_peer);
void nanotorrent_peer_send_data_request(
		const nanotorrent_peer_info_t *remote_peer, uint8_t piece_index,
		uint16_t data_start);

void nanotorrent_peer_handle_request_retry(nanotorrent_retry_event_t event,
		void *data);

void nanotorrent_peer_handle_message(struct udp_socket *peer_socket, void *ptr,
		const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen);

void nanotorrent_peer_init() {
	// Initialize peers
	nanotorrent_peer_conn_t *conn;
	ARRAY_FOR(conn, state.exchange.peers.all, NANOTORRENT_MAX_EXCHANGE_PEERS)
	{
		conn->is_valid = false;
	}
	// Initialize requests
	nanotorrent_piece_request_t *request;
	ARRAY_FOR(request, state.exchange.requests, NANOTORRENT_MAX_PEER_REQUESTS)
	{
		request->is_valid = false;
		nanotorrent_retry_init(&request->retry, NANOTORRENT_PEER_RETRY_TIMEOUT,
				nanotorrent_peer_handle_request_retry);
	}
	// Register peer socket
	udp_socket_close(&peer_socket);
	udp_socket_register(&peer_socket, NULL, nanotorrent_peer_handle_message);
	udp_socket_bind(&peer_socket, NANOTORRENT_PEER_PORT);
	PRINTF("Listening for peers on port %u\n", NANOTORRENT_PEER_PORT);
}

void nanotorrent_peer_shutdown() {
	// Close peer socket
	udp_socket_close(&peer_socket);
}

uint8_t nanotorrent_peer_count() {
	uint8_t count = 0;
	nanotorrent_peer_conn_t *conn;
	ARRAY_FOR(conn, state.exchange.peers.all, NANOTORRENT_MAX_EXCHANGE_PEERS)
	{
		if (conn->is_valid) {
			count++;
		}
	}
	return count;
}

nanotorrent_peer_conn_t *nanotorrent_peer_find(
		const nanotorrent_peer_info_t *peer_info) {
	nanotorrent_peer_conn_t *conn;
	ARRAY_FOR(conn, state.exchange.peers.all, NANOTORRENT_MAX_EXCHANGE_PEERS)
	{
		if (conn->is_valid
				&& nanotorrent_peer_info_cmp(peer_info, &conn->peer_info)) {
			return conn;
		}
	}
	return NULL;
}

nanotorrent_peer_conn_t *nanotorrent_peer_find_out_slot() {
	nanotorrent_peer_conn_t *conn;
	// Find first available connection slot
	ARRAY_FOR(conn, state.exchange.peers.out, NANOTORRENT_MAX_OUT_PEERS)
	{
		if (!conn->is_valid) {
			return conn;
		}
	}
	return NULL;
}

nanotorrent_peer_conn_t *nanotorrent_peer_find_in_slot() {
	nanotorrent_peer_conn_t *conn;
	// Find first available connection conn
	ARRAY_FOR(conn, state.exchange.peers.in, NANOTORRENT_MAX_IN_PEERS)
	{
		if (!conn->is_valid) {
			return conn;
		}
	}
	return NULL;
}

void nanotorrent_peer_add(nanotorrent_peer_conn_t *conn) {
	conn->is_valid = true;
	conn->have = 0;
	etimer_set(&conn->heartbeat, NANOTORRENT_PEER_HEARTBEAT_TIMEOUT);
}

void nanotorrent_peer_remove(nanotorrent_peer_conn_t *conn) {
	conn->is_valid = false;
	etimer_stop(&conn->heartbeat);
}

nanotorrent_peer_conn_t *nanotorrent_peer_connect(
		const nanotorrent_peer_info_t *peer_info) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer_info);
	if (conn != NULL) {
		// Already connected with this peer
		return conn;
	}
	// Find available outgoing slot
	conn = nanotorrent_peer_find_out_slot();
	if (conn == NULL) {
		// No more slots available
		return NULL;
	}
	// Add peer connection
	conn->peer_info = *peer_info;
	nanotorrent_peer_add(conn);
	return conn;
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
		const nanotorrent_peer_info_t *peer_info) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer_info);
	if (conn != NULL) {
		// Already connected with this peer
		return conn;
	}
	// Find available incoming slot
	conn = nanotorrent_peer_find_in_slot();
	if (conn == NULL) {
		// No more slots available
		return NULL;
	}
	// Add peer connection
	conn->peer_info = *peer_info;
	nanotorrent_peer_add(conn);
	return conn;
}

bool nanotorrent_peer_force_disconnect(const nanotorrent_peer_info_t *peer_info) {
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_find(peer_info);
	if (conn == NULL) {
		return false;
	}
	// Remove peer connection
	nanotorrent_peer_remove(conn);
	return true;
}

bool nanotorrent_peer_disconnect(const nanotorrent_peer_info_t *peer_info) {
	if (!nanotorrent_peer_force_disconnect(peer_info)) {
		return false;
	}
	// TODO Send CLOSE message
	return true;
}

nanotorrent_piece_request_t *nanotorrent_peer_add_request() {
	nanotorrent_piece_request_t *request;
	ARRAY_FOR(request, state.exchange.requests, NANOTORRENT_MAX_PEER_REQUESTS)
	{
		if (!request->is_valid) {
			return request;
		}
	}
	return NULL;
}

nanotorrent_piece_request_t *nanotorrent_peer_find_request(uint8_t piece_index) {
	nanotorrent_piece_request_t *request;
	ARRAY_FOR(request, state.exchange.requests, NANOTORRENT_MAX_PEER_REQUESTS)
	{
		if (request->is_valid && request->index == piece_index) {
			return request;
		}
	}
	return NULL;
}

void nanotorrent_peer_request_start(nanotorrent_piece_request_t *request) {
	request->is_valid = true;
	nanotorrent_retry_start(&request->retry, NANOTORRENT_MAX_PEER_RETRIES,
			request);
}

void nanotorrent_peer_request_cancel(nanotorrent_piece_request_t *request) {
	request->is_valid = false;
	nanotorrent_retry_stop(&request->retry);
}

bool nanotorrent_peer_should_receive_data(
		const nanotorrent_peer_info_t *peer_info, uint8_t piece_index,
		uint16_t data_offset, uint16_t data_length) {
	// Ignore if we already have piece
	if (nanotorrent_piece_is_complete(piece_index)) {
		return false;
	}
	// Already requesting this piece?
	nanotorrent_piece_request_t *request;
	request = nanotorrent_peer_find_request(piece_index);
	if (request != NULL) {
		// Data in range of current request?
		if (data_offset <= request->offset
				&& request->offset <= data_offset + data_length) {
			// Data valid for current request
			return true;
		}
	} else {
		// Opportunistic: try to add request for this data
		nanotorrent_peer_conn_t *conn;
		conn = nanotorrent_peer_connect(peer_info);
		if (conn == NULL) {
			return false;
		}
		request = nanotorrent_peer_add_request();
		if (request == NULL) {
			return false;
		}
		request->peer = *peer_info;
		request->index = piece_index;
		request->offset = data_offset + data_length;
		return true;
	}
	return false;
}

void nanotorrent_peer_data_received(const nanotorrent_peer_info_t *peer_info,
		uint8_t piece_index, uint16_t data_offset, uint16_t data_length) {
	nanotorrent_piece_request_t *request;
	request = nanotorrent_peer_find_request(piece_index);
	if (request == NULL) {
		ERROR("Request should not be null for piece %u", piece_index);
		return;
	}
	// Change request uploader
	request->peer = *peer_info;
	// Is piece completed?
	uint16_t piece_size = nanotorrent_piece_size(&state.desc.info, piece_index);
	uint16_t next_offset = data_offset + data_length;
	if (next_offset < piece_size) {
		// Start next request
		request->offset = next_offset;
		nanotorrent_peer_request_start(request);
	} else {
		// Piece fully received
		sha1_context_t context;
		bool is_complete = nanotorrent_piece_verify(&context, piece_index);
		nanotorrent_piece_set_complete(piece_index, is_complete);
		if (is_complete) {
			// Piece completed
			NOTE("Piece %u completed", piece_index);
			nanotorrent_peer_request_cancel(request);
			// TODO Select next piece
		} else {
			// Piece corrupted
			WARN("Piece %u corrupted", piece_index);
			// Restart request
			request->offset = 0;
			nanotorrent_peer_request_start(request);
		}
	}
}

void nanotorrent_peer_handle_request_retry(nanotorrent_retry_event_t event,
		void *data) {
	nanotorrent_piece_request_t *request = data;
	if (!request->is_valid)
		return;

	switch (event) {
	case RETRY_AGAIN:
		// Try again
		// TODO Send piece request
		break;
	case RETRY_STOP:
		// Stopped retrying
		// Cancel request
		nanotorrent_peer_request_cancel(request);
		break;
	}
}

void nanotorrent_peer_send_message(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *remote_peer) {
	udp_socket_sendto(&peer_socket, buffer, buffer_length,
			&remote_peer->peer_ip, NANOTORRENT_PEER_PORT);
}

CC_INLINE void nanotorrent_peer_make_header(
		nanotorrent_peer_message_header_t *header, uint8_t type) {
	sha1_copy(&header->info_hash, &state.info_hash);
	header->type = type;
	header->have = state.piece.have;
}

void nanotorrent_peer_send_close(const nanotorrent_peer_info_t *remote_peer) {
	nanotorrent_peer_close_t message;
	nanotorrent_peer_make_header(&message.header, NANOTRACKER_PEER_CLOSE);

	uint8_t buffer[sizeof(message)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_close(&cur, &message);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, remote_peer);
}

void nanotorrent_peer_send_have(const nanotorrent_peer_info_t *remote_peer) {
	nanotorrent_peer_have_t message;
	nanotorrent_peer_make_header(&message.header, NANOTRACKER_PEER_CLOSE);

	uint8_t buffer[sizeof(message)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_have(&cur, &message);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, remote_peer);
}

void nanotorrent_peer_send_data_request(
		const nanotorrent_peer_info_t *remote_peer, uint8_t piece_index,
		uint16_t data_start) {
	nanotorrent_peer_data_t request;
	nanotorrent_peer_make_header(&request.header,
			NANOTRACKER_PEER_DATA_REQUEST);
	request.piece_index = piece_index;
	request.data_start = data_start;

	uint8_t buffer[sizeof(request)];
	uint8_t *cur = buffer;
	nanotorrent_pack_peer_data(&cur, &request);
	uint16_t length = cur - buffer;

	nanotorrent_peer_send_message(buffer, length, remote_peer);
}

uint16_t nanotorrent_peer_write_data_reply(uint8_t **cur, uint16_t buffer_size,
		uint8_t piece_index, uint16_t data_start) {
	// Write header
	nanotorrent_peer_data_t reply;
	nanotorrent_peer_make_header(&reply.header, NANOTRACKER_PEER_DATA_REPLY);
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
		// TODO Don't return negative value for unsigned return type
		return -1;
	}
	*cur += data_length;

	// Return number of data bytes in reply
	return data_length;
}

void nanotorrent_peer_handle_data_request(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *remote_peer) {
	nanotorrent_peer_data_t request;

	// Parse request header
	const uint8_t *cur = buffer;
	nanotorrent_unpack_peer_data(&cur, &request);

	// Check if we have requested piece
	if (!nanotorrent_piece_is_complete(request.piece_index)) {
		WARN("Peer requested incomplete piece %u", request.piece_index);
		return;
	}

	// Reply with (part of) requested data
	// TODO Send multiple parts with timer?
	uint8_t reply_buffer[NANOTORRENT_MAX_UDP_PAYLOAD_SIZE];
	uint8_t *reply_cur = reply_buffer;
	uint16_t data_length = nanotorrent_peer_write_data_reply(&reply_cur,
			sizeof(reply_buffer), request.piece_index, request.data_start);
	if (data_length < 0) {
		return;
	}

	// Send reply
	uint16_t reply_length = reply_cur - reply_buffer;
	nanotorrent_peer_send_message(reply_buffer, reply_length, remote_peer);
}

void nanotorrent_peer_handle_data_reply(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *remote_peer) {
	nanotorrent_peer_data_t reply;

	// Parse reply header
	const uint8_t *cur = buffer;
	nanotorrent_unpack_peer_data(&cur, &reply);
	uint16_t header_length = cur - buffer;
	uint16_t data_length = buffer_length - header_length;

	// Check if we are interested in this data
	if (nanotorrent_peer_should_receive_data(remote_peer, reply.piece_index,
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
	nanotorrent_peer_data_received(remote_peer, reply.piece_index,
			reply.data_start, written);
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
	nanotorrent_peer_info_t remote_peer;
	uip_ip6addr_copy(&remote_peer.peer_ip, src_addr);

	if (header.type == NANOTRACKER_PEER_CLOSE) {
		// Handle close immediately
		nanotorrent_peer_force_disconnect(&remote_peer);
		return;
	} else if (header.type == NANOTRACKER_PEER_DATA_REPLY) {
		// Handle data reply first
		// This can add opportunistic connections and requests
		nanotorrent_peer_handle_data_reply(data, datalen, &remote_peer);
	}

	// Accept connection
	nanotorrent_peer_conn_t *conn;
	conn = nanotorrent_peer_accept(&remote_peer);
	if (conn == NULL) {
		WARN("Cannot accept peer");
		PRINT6ADDR(&remote_peer.peer_ip);
		return;
	}

	// Update peer state
	conn->have = header.have;
	etimer_restart(&conn->heartbeat);
	// TODO Schedule new requests?

	switch (header.type) {
	case NANOTRACKER_PEER_HAVE:
	case NANOTRACKER_PEER_DATA_REPLY:
		// Already handled
		break;
	case NANOTRACKER_PEER_DATA_REQUEST:
		// Reply to data request
		nanotorrent_peer_handle_data_request(data, datalen, &remote_peer);
		break;
	default:
		WARN("Ignoring peer message with unknown type %u", header.type);
	}

}
