/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "basic.h"

void nanotorrent_pack_uint8(uint8_t **cur, const uint8_t *num) {
	**cur = *num;
	**cur += sizeof(*num);
}

void nanotorrent_unpack_uint8(uint8_t **cur, uint8_t *num) {
	*num = **cur;
	**cur += sizeof(*num);
}

void nanotorrent_pack_uint16(uint8_t **cur, const uint16_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 8;
	buf[1] = (*num);
	**cur += sizeof(*num);
}

void nanotorrent_unpack_uint16(uint8_t **cur, uint16_t *num) {
	uint8_t *buf = *cur;
	*num = buf[0] << 8 | buf[1];
	**cur += sizeof(*num);
}

void nanotorrent_pack_uint32(uint8_t **cur, const uint32_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 24;
	buf[1] = (*num) >> 16;
	buf[2] = (*num) >> 8;
	buf[3] = (*num);
	**cur += sizeof(*num);
}

void nanotorrent_unpack_uint32(uint8_t **cur, uint32_t *num) {
	uint8_t *buf = *cur;
	*num = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
	**cur += sizeof(*num);
}

void nanotorrent_pack_ip6addr(uint8_t **cur, const uip_ip6addr_t *addr) {
	uip_ip4addr_copy(*cur, addr);
	*cur += sizeof(*addr);
}

void nanotorrent_unpack_ip6addr(uint8_t **cur, uip_ip6addr_t *addr) {
	uip_ip4addr_copy(addr, *cur);
	*cur += sizeof(*addr);
}

void nanotorrent_pack_sha1_digest(uint8_t **cur, const sha1_digest_t *digest) {
	sha1_copy(*cur, digest);
	*cur += sizeof(*digest);
}

void nanotorrent_unpack_sha1_digest(uint8_t **cur, sha1_digest_t *digest) {
	sha1_copy(digest, *cur);
	*cur += sizeof(*digest);
}
