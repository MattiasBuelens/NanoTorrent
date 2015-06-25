/**
 * \file
 *         Packing/unpacking of swarm network packets.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-swarm.h"
#include "pack-basic.h"

uint8_t *nanotorrent_pack_peer_info(uint8_t *buf,
		const nanotorrent_peer_info_t *peer_info) {
	buf = nanotorrent_pack_ip6addr(buf, &peer_info->peer_ip);
	return buf;
}

const uint8_t *nanotorrent_unpack_peer_info(const uint8_t *buf,
		nanotorrent_peer_info_t *peer_info) {
	buf = nanotorrent_unpack_ip6addr(buf, &peer_info->peer_ip);
	return buf;
}

uint8_t *nanotorrent_pack_announce_request(uint8_t *buf,
		const nanotorrent_announce_request_t *request) {
	buf = nanotorrent_pack_sha1_digest(buf, &request->info_hash);
	buf = nanotorrent_pack_uint8(buf, &request->num_want);
	buf = nanotorrent_pack_uint8(buf, &request->event);
	return buf;
}

const uint8_t *nanotorrent_unpack_announce_request(const uint8_t *buf,
		nanotorrent_announce_request_t *request) {
	buf = nanotorrent_unpack_sha1_digest(buf, &request->info_hash);
	buf = nanotorrent_unpack_uint8(buf, &request->num_want);
	buf = nanotorrent_unpack_uint8(buf, &request->event);
	return buf;
}

uint8_t *nanotorrent_pack_announce_reply(uint8_t *buf,
		const nanotorrent_announce_reply_t *reply) {
	buf = nanotorrent_pack_sha1_digest(buf, &reply->info_hash);
	buf = nanotorrent_pack_uint8(buf, &reply->num_peers);
	return buf;
}

const uint8_t *nanotorrent_unpack_announce_reply(const uint8_t *buf,
		nanotorrent_announce_reply_t *reply) {
	buf = nanotorrent_unpack_sha1_digest(buf, &reply->info_hash);
	buf = nanotorrent_unpack_uint8(buf, &reply->num_peers);
	return buf;
}
