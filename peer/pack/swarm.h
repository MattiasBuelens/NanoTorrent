/**
 * \file
 *         Packing/unpacking of swarm network packets.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_SWARM_H_
#define NANOTORRENT_PACK_SWARM_H_

#include "../swarm.h"

void nanotorrent_pack_peer_info(uint8_t **cur,
		const nanotorrent_peer_info_t *peer_info);

void nanotorrent_unpack_peer_info(uint8_t **cur,
		nanotorrent_peer_info_t *peer_info);

void nanotorrent_pack_announce_request(uint8_t **cur,
		const nanotorrent_announce_request_t *request);

void nanotorrent_unpack_announce_request(uint8_t **cur,
		nanotorrent_announce_request_t *request);

void nanotorrent_pack_announce_reply(uint8_t **cur,
		const nanotorrent_announce_reply_t *reply);

void nanotorrent_unpack_announce_reply(uint8_t **cur,
		nanotorrent_announce_reply_t *reply);

#endif /* NANOTORRENT_PACK_COMMON_H_ */
