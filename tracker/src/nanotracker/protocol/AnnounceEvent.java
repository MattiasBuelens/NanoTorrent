package nanotracker.protocol;

public enum AnnounceEvent {

	/**
	 * Peer refreshes its swarm membership.
	 */
	REFRESH(0),

	/**
	 * Peer starting.
	 */
	STARTED(1),

	/**
	 * Peer shutting down gracefully.
	 */
	STOPPED(2),

	/**
	 * Peer completed download.
	 */
	COMPLETED(3);

	private final int value;

	private AnnounceEvent(int value) {
		this.value = value;
	}

	public int getValue() {
		return value;
	}

	public static AnnounceEvent byValue(int value) {
		for (AnnounceEvent event : AnnounceEvent.values()) {
			if (event.getValue() == value) {
				return event;
			}
		}
		throw new IllegalArgumentException("Unknown announce event: " + value);
	}

}
