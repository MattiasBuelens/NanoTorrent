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
 * Enable tracker.
 *
 * - Peers discover and connect with remote peers.
 * - Peers keep the tracker up-to-date on their
 *   swarm membership state.
 */
#define NANOTORRENT_TRACKER 1

/**
 * Enable link-local optimizations
 *
 * - Peers discover and connect with local peers.
 * - Peers send piece data to local peers as link-local multicasts,
 *   so other local peers can benefit from the exchange.
 */
#define NANOTORRENT_LOCAL 1

/**
 * Use the interface identifier (IID) of a peer's
 * IPv6 address as the peer's identifier.
 *
 * By default, peers are identified by their IPv6 address.
 * However, this may cause TWO connections to be created
 * for a single local peer:
 * - One connection uses the peer's global unicast address
 *   as reported by the tracker
 * - One connection uses the peer's link-local address
 *   as discovered with link-local multicast
 *
 * To prevent overloading local peers with doubled connections,
 * we can use the 64-bit IID as a 'supercookie' to identify
 * a peer across multiple networks IF the peer uses the same
 * IID for both its global unicast and link-local addresses.
 *
 * This assumption holds if peers only use addresses derived
 * from their MAC addresses through stateless address
 * auto-configuration. When peers use a temporary address
 * (generated locally or obtained through DHCP), the IIDs of
 * both addresses no longer match up and accidental IID
 * collisions may occur between two distinct peers.
 */
#define NANOTORRENT_IID_AS_PEER_ID 0

/**
 * Minimum completion before allowing end-game mode
 *
 * Completion is number of completed pieces
 * divided by total number of pieces
 */
#define NANOTORRENT_END_GAME_MIN_COMPLETION 0.8f

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
#define NANOTORRENT_ANNOUNCE_RETRY_TIMEOUT (3ul * CLOCK_SECOND)

/**
 * Time between two announce refresh requests
 */
#define NANOTORRENT_ANNOUNCE_REFRESH_PERIOD (60ul * CLOCK_SECOND)

/**
 * Maximum number of retries for a peer data request
 */
#define NANOTORRENT_MAX_PEER_RETRIES 3

/**
 * Base timeout for retrying a peer data request
 */
#define NANOTORRENT_PEER_RETRY_TIMEOUT (1ul * CLOCK_SECOND)

/**
 * Time between two heartbeats to connected peers
 */
#define NANOTORRENT_PEER_HEARTBEAT_PERIOD (15ul * CLOCK_SECOND)

/**
 * Timeout for a heartbeat to be expired
 */
#define NANOTORRENT_PEER_HEARTBEAT_TIMEOUT (3 * NANOTORRENT_PEER_HEARTBEAT_PERIOD)

/**
 * Time to keep seeding after completing torrent
 */
#define NANOTORRENT_SEED_TIME (300ul * CLOCK_SECOND)

#endif /* NANOTORRENT_CONFIG_H_ */
