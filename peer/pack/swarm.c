/**
 * \file
 *         Packing/unpacking of swarm network packets.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack/swarm.h"

#include "basic.h"

void nanotorrent_pack_peer_info(uint8_t **cur,
		const nanotorrent_peer_info_t *peer_info) {
	nanotorrent_pack_ip6addr(cur, &peer_info->peer_ip);
	nanotorrent_pack_uint16(cur, &peer_info->peer_port);
}

void nanotorrent_unpack_peer_info(uint8_t **cur,
		nanotorrent_peer_info_t *peer_info) {
	nanotorrent_unpack_ip6addr(cur, &peer_info->peer_ip);
	nanotorrent_unpack_uint16(cur, &peer_info->peer_port);
}

void nanotorrent_pack_announce_request(uint8_t **cur,
		const nanotorrent_announce_request_t *request) {
	nanotorrent_pack_sha1_digest(cur, &request->info_hash);
	nanotorrent_pack_peer_info(cur, &request->peer_info);
	nanotorrent_pack_uint8(cur, &request->num_want);
	nanotorrent_pack_uint8(cur, &request->event);
}

void nanotorrent_unpack_announce_request(uint8_t **cur,
		nanotorrent_announce_request_t *request) {
	nanotorrent_unpack_sha1_digest(cur, &request->info_hash);
	nanotorrent_unpack_peer_info(cur, &request->peer_info);
	nanotorrent_unpack_uint8(cur, &request->num_want);
	nanotorrent_unpack_uint8(cur, &request->event);
}

void nanotorrent_pack_announce_reply(uint8_t **cur,
		const nanotorrent_announce_reply_t *reply) {
	nanotorrent_pack_sha1_digest(cur, &reply->info_hash);
	nanotorrent_pack_uint8(cur, &reply->num_peers);
}

void nanotorrent_unpack_announce_reply(uint8_t **cur,
		nanotorrent_announce_reply_t *reply) {
	nanotorrent_unpack_sha1_digest(cur, &reply->info_hash);
	nanotorrent_unpack_uint8(cur, &reply->num_peers);
}
