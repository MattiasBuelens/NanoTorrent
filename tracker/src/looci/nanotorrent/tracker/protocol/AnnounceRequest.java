package looci.nanotorrent.tracker.protocol;

import java.net.Inet6Address;
import java.nio.ByteBuffer;

public class AnnounceRequest {

	private final InfoHash infoHash;
	private final Inet6Address ip;
	private final short port;
	private final int numWant;
	private final AnnounceEvent event;

	protected AnnounceRequest(Builder builder) {
		this.infoHash = builder.infoHash;
		this.ip = builder.ip;
		this.port = builder.port;
		this.numWant = builder.numWant;
		this.event = builder.event;
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public Inet6Address getIP() {
		return ip;
	}

	public short getPort() {
		return port;
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
		builder.ip(IOUtils.getInet6Address(data));
		builder.port(data.getShort());
		builder.numWant(data.getInt());
		builder.event(AnnounceEvent.byValue(data.get()));
		return builder.build();
	}

	public static class Builder {

		private InfoHash infoHash;
		private Inet6Address ip;
		private short port;
		private int numWant;
		private AnnounceEvent event;

		public Builder infoHash(InfoHash infoHash) {
			this.infoHash = infoHash;
			return this;
		}

		public Builder ip(Inet6Address ip) {
			this.ip = ip;
			return this;
		}

		public Builder port(short port) {
			this.port = port;
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
