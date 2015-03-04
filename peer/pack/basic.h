/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_PACK_BASIC_H_
#define NANOTORRENT_PACK_BASIC_H_

#include "../common.h"

void nanotorrent_pack_uint8(uint8_t **cur, const uint8_t *num);

void nanotorrent_unpack_uint8(uint8_t **cur, uint8_t *num);

void nanotorrent_pack_uint16(uint8_t **cur, const uint16_t *num);

void nanotorrent_unpack_uint16(uint8_t **cur, uint16_t *num);

void nanotorrent_pack_uint32(uint8_t **cur, const uint32_t *num);

void nanotorrent_unpack_uint32(uint8_t **cur, uint32_t *num);

void nanotorrent_pack_ip6addr(uint8_t **cur, const uip_ip6addr_t *addr);

void nanotorrent_unpack_ip6addr(uint8_t **cur, uip_ip6addr_t *addr);

void nanotorrent_pack_sha1_digest(uint8_t **cur, const sha1_digest_t *digest);

void nanotorrent_unpack_sha1_digest(uint8_t **cur, sha1_digest_t *digest);

#endif /* NANOTORRENT_PACK_BASIC_H_ */
