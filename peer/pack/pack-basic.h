/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_BASIC_H_
#define NANOTORRENT_PACK_BASIC_H_

#include "../common.h"

uint8_t *nanotorrent_pack_uint8(uint8_t *buf, const uint8_t *num);

const uint8_t *nanotorrent_unpack_uint8(const uint8_t *buf, uint8_t *num);

uint8_t *nanotorrent_pack_uint16(uint8_t *buf, const uint16_t *num);

const uint8_t *nanotorrent_unpack_uint16(const uint8_t *buf, uint16_t *num);

uint8_t *nanotorrent_pack_uint32(uint8_t *buf, const uint32_t *num);

const uint8_t *nanotorrent_unpack_uint32(const uint8_t *buf, uint32_t *num);

uint8_t *nanotorrent_pack_ip6addr(uint8_t *buf, const uip_ip6addr_t *addr);

const uint8_t *nanotorrent_unpack_ip6addr(const uint8_t *buf,
		uip_ip6addr_t *addr);

uint8_t *nanotorrent_pack_sha1_digest(uint8_t *buf, const sha1_digest_t *digest);

const uint8_t *nanotorrent_unpack_sha1_digest(const uint8_t *buf,
		sha1_digest_t *digest);

#endif /* NANOTORRENT_PACK_BASIC_H_ */
