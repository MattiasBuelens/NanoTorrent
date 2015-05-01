/**
 * \file
 *         Retry handling.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "retry.h"

void nanotorrent_retry_init(nanotorrent_retry_t *retry, clock_time_t interval,
		nanotorrent_retry_callback_t callback) {
	retry->callback = callback;
	retry->interval = interval;
	nanotorrent_retry_stop(retry);
}

void nanotorrent_retry_next(nanotorrent_retry_t *retry) {
	if (retry->remaining > 1) {
		// Reset for next try
		etimer_reset(&retry->timer);
	} else {
		// Stop
		nanotorrent_retry_stop(retry);
	}
	if (retry->remaining > 0) {
		// Try again
		retry->remaining--;
		retry->callback(RETRY_AGAIN);
	} else {
		// Stop retrying
		retry->callback(RETRY_STOP);
	}
}

void nanotorrent_retry_start(nanotorrent_retry_t *retry, uint8_t retries) {
	// Start timer and set count
	etimer_set(&retry->timer, retry->interval);
	retry->remaining = retries;
	// Try immediately
	nanotorrent_retry_next(retry);
}

void nanotorrent_retry_stop(nanotorrent_retry_t *retry) {
	// Stop timer and reset count
	etimer_stop(&retry->timer);
	retry->remaining = 0;
}

bool nanotorrent_retry_check(nanotorrent_retry_t *retry) {
	return retry->remaining > 0 && etimer_expired(&retry->timer);
}

void nanotorrent_retry_process(nanotorrent_retry_t *retry) {
	if (nanotorrent_retry_check(retry)) {
		nanotorrent_retry_next(retry);
	}
}

