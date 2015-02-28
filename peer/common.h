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

#include "contiki.h"
#include "uip.h"

#include "config.h"
#include "sha1.h"

#endif /* NANOTORRENT_COMMON_H_ */
