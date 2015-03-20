package looci.nanotorrent.tracker.protocol;

public enum AnnounceEvent {

	/**
	 * Peer refreshes its swarm membership.
	 */
	REFRESH((byte) 0),

	/**
	 * Peer starting.
	 */
	STARTED((byte) 1),

	/**
	 * Peer shutting down gracefully.
	 */
	STOPPED((byte) 2),

	/**
	 * Peer completed download.
	 */
	COMPLETED((byte) 3);

	private final byte value;

	private AnnounceEvent(byte value) {
		this.value = value;
	}

	public byte getValue() {
		return value;
	}

	public static AnnounceEvent byValue(byte value) {
		for (AnnounceEvent event : AnnounceEvent.values()) {
			if (event.getValue() == value) {
				return event;
			}
		}
		throw new IllegalArgumentException("Unknown announce event: " + value);
	}

}
