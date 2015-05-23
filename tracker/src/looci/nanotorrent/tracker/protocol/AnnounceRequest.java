package looci.nanotorrent.tracker.protocol;

import java.nio.ByteBuffer;

public class AnnounceRequest {

	private final InfoHash infoHash;
	private final int numWant;
	private final AnnounceEvent event;

	protected AnnounceRequest(Builder builder) {
		this.infoHash = builder.infoHash;
		this.numWant = builder.numWant;
		this.event = builder.event;
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public int getNumWant() {
		return numWant;
	}

	public AnnounceEvent getEvent() {
		return event;
	}

	public static AnnounceRequest read(ByteBuffer data) {
		Builder builder = new Builder();
		builder.infoHash(InfoHash.read(data));
		builder.numWant(data.get() & 0xFF);
		builder.event(AnnounceEvent.byValue(data.get() & 0xFF));
		return builder.build();
	}

	public static class Builder {

		private InfoHash infoHash;
		private int numWant;
		private AnnounceEvent event;

		public Builder infoHash(InfoHash infoHash) {
			this.infoHash = infoHash;
			return this;
		}

		public Builder numWant(int numWant) {
			this.numWant = numWant;
			return this;
		}

		public Builder event(AnnounceEvent event) {
			this.event = event;
			return this;
		}

		public AnnounceRequest build() {
			return new AnnounceRequest(this);
		}

	}

}
