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

void nanotorrent_peer_handle_message(struct udp_socket *peer_socket, void *ptr,
		const uip_ipaddr_t *src_addr, uint16_t src_port,
		const uip_ipaddr_t *dest_addr, uint16_t dest_port, const uint8_t *data,
		uint16_t datalen);

void nanotorrent_peer_init() {
	// Register peer socket
	struct udp_socket *socket = &state.peer.peer_socket;
	udp_socket_close(socket);
	udp_socket_register(socket, NULL, nanotorrent_peer_handle_message);
	// Store listen port
	state.listen_port = socket->udp_conn->lport;
}

void nanotorrent_peer_shutdown() {
	// Close peer socket
	udp_socket_close(&state.peer.peer_socket);
}

void nanotorrent_peer_send_message(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *remote_peer) {
	udp_socket_sendto(&state.peer.peer_socket, buffer, buffer_length,
			&remote_peer->peer_ip, remote_peer->peer_port);
}

void nanotorrent_peer_handle_data_request(const uint8_t *buffer,
		uint16_t buffer_length, const nanotorrent_peer_info_t *remote_peer) {
	nanotorrent_peer_data_request_t request;

	// Parse request header
	const uint8_t *cur = buffer;
	nanotorrent_unpack_peer_data_request(&cur, &request);

	// Check if we have requested piece
	if (!nanotorrent_piece_is_complete(request.piece_index)) {
		WARN("Peer requested incomplete piece %u", request.piece_index);
		return;
	}

	// Reply with (part of) requested data
	// TODO Send multiple parts with timer?
	uint8_t reply_buffer[5];
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
	remote_peer.peer_port = src_port;

	switch (header.type) {
	case NANOTRACKER_PEER_CLOSE:
		// TODO Close peer connection
		break;
	case NANOTRACKER_PEER_HAVE:
		// TODO Create peer connection if not exists
		break;
	case NANOTRACKER_PEER_DATA_REQUEST:
		nanotorrent_peer_handle_data_request(data, datalen, &remote_peer);
		break;
	case NANOTRACKER_PEER_DATA_REPLY:
		break;
	default:
		WARN("Ignoring peer message with unknown type %u", header.type);
	}

}

CC_INLINE void nanotorrent_peer_make_header(
		nanotorrent_peer_message_header_t *header, uint8_t type) {
	sha1_copy(&header->info_hash, &state.info_hash);
	header->type = type;
}

void nanotorrent_peer_write_close(uint8_t **cur) {
	nanotorrent_peer_close_t message;
	nanotorrent_peer_make_header(&message.header, NANOTRACKER_PEER_CLOSE);

	nanotorrent_pack_peer_close(cur, &message);
}

void nanotorrent_peer_write_have(uint8_t **cur) {
	nanotorrent_peer_have_t message;
	nanotorrent_peer_make_header(&message.header, NANOTRACKER_PEER_HAVE);
	message.have = state.piece.have;

	nanotorrent_pack_peer_have(cur, &message);
}

void nanotorrent_peer_write_data_request(uint8_t **cur, uint8_t piece_index,
		uint16_t data_start) {
	nanotorrent_peer_data_request_t request;
	nanotorrent_peer_make_header(&request.header,
			NANOTRACKER_PEER_DATA_REQUEST);
	request.piece_index = piece_index;
	request.data_start = data_start;

	nanotorrent_pack_peer_data_request(cur, &request);
}

uint16_t nanotorrent_peer_write_data_reply(uint8_t **cur, uint16_t buffer_size,
		uint8_t piece_index, uint16_t data_start) {
	// Write initial reply header with zero data length
	nanotorrent_peer_data_reply_t reply;
	nanotorrent_peer_make_header(&reply.header, NANOTRACKER_PEER_DATA_REPLY);
	reply.piece_index = piece_index;
	reply.data_start = data_start;
	reply.data_length = 0;

	uint8_t *header_start = *cur;
	nanotorrent_pack_peer_data_reply(cur, &reply);
	uint8_t *header_end = *cur;
	uint16_t header_length = header_end - header_start;

	// Write data
	uint16_t data_length = nanotorrent_piece_read(piece_index, data_start, *cur,
			buffer_size - header_length);
	if (data_length < 0) {
		return -1;
	}
	*cur += data_length;

	// Write actual data length
	uint8_t *data_length_ptr = header_end - sizeof(data_length);
	nanotorrent_pack_uint16(&data_length_ptr, &data_length);

	return data_length;
}
