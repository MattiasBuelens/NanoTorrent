package looci.nanotorrent.tracker.protocol;

import java.nio.ByteBuffer;

public class AnnounceRequest {

	private final InfoHash infoHash;
	private final PeerInfo peerInfo;
	private final int numWant;
	private final AnnounceEvent event;

	protected AnnounceRequest(Builder builder) {
		this.infoHash = builder.infoHash;
		this.peerInfo = builder.peerInfo;
		this.numWant = builder.numWant;
		this.event = builder.event;
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public PeerInfo getPeerInfo() {
		return peerInfo;
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
		builder.peerInfo(PeerInfo.read(data));
		builder.numWant(data.getInt());
		builder.event(AnnounceEvent.byValue(data.get()));
		return builder.build();
	}

	public static class Builder {

		private InfoHash infoHash;
		private PeerInfo peerInfo;
		private int numWant;
		private AnnounceEvent event;

		public Builder infoHash(InfoHash infoHash) {
			this.infoHash = infoHash;
			return this;
		}

		public Builder peerInfo(PeerInfo peerInfo) {
			this.peerInfo = peerInfo;
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
