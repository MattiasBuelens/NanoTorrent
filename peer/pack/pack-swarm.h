/**
 * \file
 *         Packing/unpacking of swarm network packets.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_SWARM_H_
#define NANOTORRENT_PACK_SWARM_H_

#include "../swarm.h"

uint8_t *nanotorrent_pack_peer_info(uint8_t *buf,
		const nanotorrent_peer_info_t *peer_info);

const uint8_t *nanotorrent_unpack_peer_info(const uint8_t *buf,
		nanotorrent_peer_info_t *peer_info);

uint8_t *nanotorrent_pack_announce_request(uint8_t *buf,
		const nanotorrent_announce_request_t *request);

const uint8_t *nanotorrent_unpack_announce_request(const uint8_t *buf,
		nanotorrent_announce_request_t *request);

uint8_t *nanotorrent_pack_announce_reply(uint8_t *buf,
		const nanotorrent_announce_reply_t *reply);

const uint8_t *nanotorrent_unpack_announce_reply(const uint8_t *buf,
		nanotorrent_announce_reply_t *reply);

#endif /* NANOTORRENT_PACK_COMMON_H_ */
