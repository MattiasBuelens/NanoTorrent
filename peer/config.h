/**
 * \file
 *         Configuration.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_CONFIG_H_
#define NANOTORRENT_CONFIG_H_

/**
 * UDP port for peer protocol
 */
#define NANOTORRENT_PEER_PORT 4242

/**
 * Maximum length of file name
 */
#define NANOTORRENT_FILE_NAME_LENGTH 16

/**
 * Maximum number of pieces in a torrent
 */
#define NANOTORRENT_MAX_PIECES 32

/**
 * Maximum amount of peers to request from tracker
 */
#define NANOTORRENT_MAX_SWARM_PEERS 5

/**
 * Maximum amount of outgoing peer connections to establish
 */
#define NANOTORRENT_MAX_OUT_PEERS NANOTORRENT_MAX_SWARM_PEERS

/**
 * Maximum amount of incoming peer connections to accept
 */
#define NANOTORRENT_MAX_IN_PEERS NANOTORRENT_MAX_OUT_PEERS

/**
 * Maximum number of retries for an announce request
 */
#define NANOTORRENT_MAX_ANNOUNCE_RETRIES 5

/**
 * Base timeout for retrying an announce request
 */
#define NANOTORRENT_ANNOUNCE_RETRY_TIMEOUT (3 * CLOCK_SECOND)

/**
 * Time between two announce refresh requests
 */
#define NANOTORRENT_ANNOUNCE_REFRESH_PERIOD (60 * CLOCK_SECOND)

/**
 * Maximum number of retries for a peer data request
 */
#define NANOTORRENT_MAX_PEER_RETRIES 3

/**
 * Base timeout for retrying a peer data request
 */
#define NANOTORRENT_PEER_RETRY_TIMEOUT (5 * CLOCK_SECOND)

/**
 * Time between two heartbeats to connected peers
 */
#define NANOTORRENT_PEER_HEARTBEAT_PERIOD (30 * CLOCK_SECOND)

/**
 * Timeout for a heartbeat to be expired
 */
#define NANOTORRENT_PEER_HEARTBEAT_TIMEOUT (3 * NANOTORRENT_PEER_HEARTBEAT_PERIOD)

#endif /* NANOTORRENT_CONFIG_H_ */
