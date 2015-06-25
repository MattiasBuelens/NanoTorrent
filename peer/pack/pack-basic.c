/**
 * \file
 *         Packing/unpacking of basic data types.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "pack-basic.h"

uint8_t * nanotorrent_pack_uint8(uint8_t *buf, const uint8_t *num) {
	*buf = *num;
	buf += sizeof(*num);
	return buf;
}

const uint8_t *nanotorrent_unpack_uint8(const uint8_t *buf, uint8_t *num) {
	*num = *buf;
	buf += sizeof(*num);
	return buf;
}

uint8_t *nanotorrent_pack_uint16(uint8_t *buf, const uint16_t *num) {
	buf[0] = (*num) >> 8;
	buf[1] = (*num);
	buf += sizeof(*num);
	return buf;
}

const uint8_t *nanotorrent_unpack_uint16(const uint8_t *buf, uint16_t *num) {
	*num = ((uint16_t) buf[0]) << 8 | buf[1];
	buf += sizeof(*num);
	return buf;
}

uint8_t *nanotorrent_pack_uint32(uint8_t *buf, const uint32_t *num) {
	buf[0] = (*num) >> 24;
	buf[1] = (*num) >> 16;
	buf[2] = (*num) >> 8;
	buf[3] = (*num);
	buf += sizeof(*num);
	return buf;
}

const uint8_t *nanotorrent_unpack_uint32(const uint8_t *buf, uint32_t *num) {
	*num = ((uint32_t) buf[0]) << 24 | ((uint32_t) buf[1]) << 16
			| ((uint32_t) buf[2]) << 8 | ((uint32_t) buf[3]);
	buf += sizeof(*num);
	return buf;
}

uint8_t *nanotorrent_pack_ip6addr(uint8_t *buf, const uip_ip6addr_t *addr) {
	uip_ip6addr_copy((uip_ip6addr_t * ) buf, addr);
	buf += sizeof(*addr);
	return buf;
}

const uint8_t *nanotorrent_unpack_ip6addr(const uint8_t *buf,
		uip_ip6addr_t *addr) {
	uip_ip6addr_copy(addr, (const uip_ip6addr_t * ) buf);
	buf += sizeof(*addr);
	return buf;
}

uint8_t *nanotorrent_pack_sha1_digest(uint8_t *buf, const sha1_digest_t *digest) {
	sha1_copy((sha1_digest_t * ) buf, digest);
	buf += sizeof(*digest);
	return buf;
}

const uint8_t *nanotorrent_unpack_sha1_digest(const uint8_t *buf,
		sha1_digest_t *digest) {
	sha1_copy(digest, (const sha1_digest_t * ) buf);
	buf += sizeof(*digest);
	return buf;
}
