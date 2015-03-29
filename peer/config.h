/**
 * \file
 *         Configuration.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_CONFIG_H_
#define NANOTORRENT_CONFIG_H_

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
#define NANOTORRENT_MAX_PEERS 16

#endif /* NANOTORRENT_CONFIG_H_ */
