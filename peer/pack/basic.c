/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "basic.h"

void nanotorrent_pack_uint8(uint8_t **cur, const uint8_t *num) {
	*(*cur++) = *num;
}

void nanotorrent_unpack_uint8(uint8_t **cur, uint8_t *num) {
	*num = *(*cur++);
}

void nanotorrent_pack_uint16(uint8_t **cur, const uint16_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 8;
	buf[1] = (*num);
	**cur += 2;
}

void nanotorrent_unpack_uint16(uint8_t **cur, uint16_t *num) {
	uint8_t *buf = *cur;
	*num = buf[0] << 8 | buf[1];
	**cur += 2;
}

void nanotorrent_pack_uint32(uint8_t **cur, const uint32_t *num) {
	uint8_t *buf = *cur;
	buf[0] = (*num) >> 24;
	buf[1] = (*num) >> 16;
	buf[2] = (*num) >> 8;
	buf[3] = (*num);
	**cur += 4;
}

void nanotorrent_unpack_uint32(uint8_t **cur, uint32_t *num) {
	uint8_t *buf = *cur;
	*num = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
	**cur += 4;
}

void nanotorrent_pack_ip6addr(uint8_t **cur, const uip_ip6addr_t *addr) {
	memcpy(*cur, addr, sizeof(uip_ip6addr_t));
	*cur += sizeof(uip_ip6addr_t);
}

void nanotorrent_unpack_ip6addr(uint8_t **cur, uip_ip6addr_t *addr) {
	memcpy(addr, *cur, sizeof(uip_ip6addr_t));
	*cur += sizeof(uip_ip6addr_t);
}

void nanotorrent_pack_sha1_digest(uint8_t **cur, const sha1_digest_t *digest) {
	memcpy(*cur, digest, sizeof(sha1_digest_t));
	*cur += sizeof(sha1_digest_t);
}

void nanotorrent_unpack_sha1_digest(uint8_t **cur, sha1_digest_t *digest) {
	memcpy(digest, *cur, sizeof(sha1_digest_t));
	*cur += sizeof(sha1_digest_t);
}
