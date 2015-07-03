/**
 * \file
 *         Retry handling.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "retry.h"

void nanotorrent_retry_init(nanotorrent_retry_t *retry, clock_time_t timeout,
		nanotorrent_retry_callback_t *again_callback,
		nanotorrent_retry_callback_t *stop_callback) {
	retry->timeout = timeout;
	retry->again_callback = again_callback;
	retry->stop_callback = stop_callback;
	retry->num_retries = 0;
	retry->max_retries = 0;
	nanotorrent_retry_stop(retry);
}

void nanotorrent_retry_next(nanotorrent_retry_t *retry) {
	if (retry->num_retries < retry->max_retries) {
		// Use exponential back-off for next timeout
		clock_time_t timeout = retry->timeout * (1 << retry->num_retries);
		etimer_set(&retry->timer, timeout);
		// Try again
		retry->num_retries++;
		retry->again_callback(retry->data);
	} else if (retry->num_retries == retry->max_retries) {
		// Stop retrying
		retry->num_retries++;
		nanotorrent_retry_stop(retry);
		retry->stop_callback(retry->data);
	}
}

void nanotorrent_retry_start(nanotorrent_retry_t *retry, uint8_t max_retries,
		void *data) {
	// Initialize
	retry->data = data;
	// Set counters
	retry->num_retries = 0;
	retry->max_retries = max_retries;
	// Try immediately
	nanotorrent_retry_next(retry);
}

void nanotorrent_retry_stop(nanotorrent_retry_t *retry) {
	// Reset
	retry->data = NULL;
	// Stop timer
	etimer_stop(&retry->timer);
}

bool nanotorrent_retry_check(nanotorrent_retry_t *retry) {
	return retry->num_retries <= retry->max_retries
			&& etimer_expired(&retry->timer);
}

void nanotorrent_retry_process(nanotorrent_retry_t *retry) {
	if (nanotorrent_retry_check(retry)) {
		nanotorrent_retry_next(retry);
	}
}

