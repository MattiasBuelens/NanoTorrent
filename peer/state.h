/**
 * \file
 *         State management.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_STATE_H_
#define NANOTORRENT_STATE_H_

#include "common.h"
#include "torrent.h"
#include "retry.h"

/**
 * Peer connection info
 */
typedef struct nanotorrent_peer_info {
	/**
	 * IPv6 address
	 */
	uip_ip6addr_t peer_ip;
} nanotorrent_peer_info_t;

#if NANOTORRENT_IID_AS_PEER_ID
#define nanotorrent_peer_info_cmp(a, b) \
	(memcmp(&(a)->peer_ip.u8[8], &(b)->peer_ip.u8[8], 8) == 0)
#else
#define nanotorrent_peer_info_cmp(a, b) \
	(uip_ip6addr_cmp(&(a)->peer_ip, &(b)->peer_ip))
#endif

/**
 * Torrent state
 */
typedef struct nanotorrent_torrent_state {
	/*
	 * Hash of torrent info
	 */
	sha1_digest_t info_hash;
	/**
	 * Destination file name
	 */
	char file_name[NANOTORRENT_FILE_NAME_LENGTH];
	/**
	 * Torrent descriptor
	 */
	nanotorrent_torrent_desc_t desc;
} nanotorrent_torrent_state_t;

/**
 * Global state.
 */
extern nanotorrent_torrent_state_t nanotorrent_state;

#endif /* NANOTORRENT_STATE_H_ */
