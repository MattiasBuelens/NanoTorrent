/**
 * \file
 *         Retry handling.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#ifndef NANOTORRENT_RETRY_H_
#define NANOTORRENT_RETRY_H_

#include "common.h"

typedef enum nanotorrent_retry_event {
	RETRY_AGAIN, RETRY_STOP
} nanotorrent_retry_event_t;

typedef void (*nanotorrent_retry_callback_t)(nanotorrent_retry_event_t event);

typedef struct nanotorrent_retry {
	/**
	 * Base timeout for exponential back-off
	 */
	clock_time_t timeout;
	/**
	 * Maximum number of remaining retries
	 */
	uint8_t max_retries;
	/**
	 * Number of retries
	 */
	uint8_t num_retries;
	/**
	 * Timer until next retry
	 */
	struct etimer timer;
	/**
	 * Callback
	 */
	nanotorrent_retry_callback_t callback;
} nanotorrent_retry_t;

void nanotorrent_retry_init(nanotorrent_retry_t *retry, clock_time_t timeout,
		nanotorrent_retry_callback_t callback);
void nanotorrent_retry_start(nanotorrent_retry_t *retry, uint8_t max_retries);
void nanotorrent_retry_stop(nanotorrent_retry_t *retry);

bool nanotorrent_retry_check(nanotorrent_retry_t *retry);
void nanotorrent_retry_process(nanotorrent_retry_t *retry);

#endif /* NANOTORRENT_RETRY_H_ */
