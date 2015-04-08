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

typedef enum {
	false, true
} bool;

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

// http://maciejczyzewski.me/2015/02/21/better-debug-notices-in-c-using-macros.html
#define NOTE(S, ...) PRINTF(                                             \
  "\x1b[1m(%s:%d, %s)\x1b[0m\n  \x1b[1m\x1b[90mnote:\x1b[0m " S "\n",    \
  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define WARN(S, ...) PRINTF(                                             \
  "\x1b[1m(%s:%d, %s)\x1b[0m\n  \x1b[1m\x1b[33mwarning:\x1b[0m " S "\n", \
  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define ERROR(S, ...) PRINTA(                                            \
  "\x1b[1m(%s:%d, %s)\x1b[0m\n  \x1b[1m\x1b[31merror:\x1b[0m " S "\n",   \
  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#include "config.h"
#include "crypto.h"

#endif /* NANOTORRENT_COMMON_H_ */
