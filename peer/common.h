/**
 * \file
 *         Common includes.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_COMMON_H_
#define NANOTORRENT_COMMON_H_

#ifndef MAX
#define MAX(a,b) ((a) > (b)? (a) : (b))
#endif /* MAX */

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a) : (b))
#endif /* MIN */

#define QUOTE_(str) #str
#define QUOTE(str) QUOTE_(str)

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

enum {
	false, true
};
typedef uint8_t bool;

/**
 * Maximum UDP payload size
 */
#define NANOTORRENT_MAX_UDP_PAYLOAD_SIZE (UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN)

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

// http://maciejczyzewski.me/2015/02/21/better-debug-notices-in-c-using-macros.html
#define NOTE(S, ...) PRINTF("" S "\n", ##__VA_ARGS__)
#define WARN(S, ...) PRINTF(                                             \
  "(%s:%d, %s)\n  Warning: " S "\n",                                     \
  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define ERROR(S, ...) PRINTF(                                            \
  "(%s:%d, %s)\n  ERROR: " S "\n",                                       \
  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#include "config.h"
#include "crypto.h"

#endif /* NANOTORRENT_COMMON_H_ */
