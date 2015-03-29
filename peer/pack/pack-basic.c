/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-basic.h"

void nanotorrent_pack_uint8(uint8_t **cur, const uint8_t *num) {
	**cur = *num;
	*cur += sizeof(*num);
}

void nanotorrent_unpack_uint8(const uint8_t **cur, uint8_t *num) {
	*num = **cur;
	*cur += sizeof(*num);
}

void nanotorrent_pack_uint16(uint8_t **cur, const uint16_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 8;
	buf[1] = (*num);
	*cur += sizeof(*num);
}

void nanotorrent_unpack_uint16(const uint8_t **cur, uint16_t *num) {
	const uint8_t *buf = *cur;
	*num = ((uint16_t) buf[0]) << 8 | buf[1];
	*cur += sizeof(*num);
}

void nanotorrent_pack_uint32(uint8_t **cur, const uint32_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 24;
	buf[1] = (*num) >> 16;
	buf[2] = (*num) >> 8;
	buf[3] = (*num);
	*cur += sizeof(*num);
}

void nanotorrent_unpack_uint32(const uint8_t **cur, uint32_t *num) {
	const uint8_t *buf = *cur;
	*num = ((uint32_t) buf[0]) << 24 | ((uint32_t) buf[1]) << 16
			| ((uint32_t) buf[2]) << 8 | ((uint32_t) buf[3]);
	*cur += sizeof(*num);
}

void nanotorrent_pack_ip6addr(uint8_t **cur, const uip_ip6addr_t *addr) {
	uip_ip6addr_copy((uip_ip6addr_t * ) *cur, addr);
	*cur += sizeof(*addr);
}

void nanotorrent_unpack_ip6addr(const uint8_t **cur, uip_ip6addr_t *addr) {
	uip_ip6addr_copy(addr, (const uip_ip6addr_t * )*cur);
	*cur += sizeof(*addr);
}

void nanotorrent_pack_sha1_digest(uint8_t **cur, const sha1_digest_t *digest) {
	sha1_copy((sha1_digest_t * ) *cur, digest);
	*cur += sizeof(*digest);
}

void nanotorrent_unpack_sha1_digest(const uint8_t **cur, sha1_digest_t *digest) {
	sha1_copy(digest, (const sha1_digest_t * ) *cur);
	*cur += sizeof(*digest);
}
