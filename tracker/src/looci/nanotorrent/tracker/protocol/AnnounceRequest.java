package looci.nanotorrent.tracker.protocol;

import java.net.Inet6Address;
import java.nio.ByteBuffer;

public class AnnounceRequest {

	private final InfoHash infoHash;
	private final PeerId peerId;
	private final short port;
	private final int uploaded;
	private final int downloaded;
	private final int left;
	private final Inet6Address ip;
	private final int numWant;
	private final AnnounceEvent event;

	protected AnnounceRequest(Builder builder) {
		this.infoHash = builder.infoHash;
		this.peerId = builder.peerId;
		this.port = builder.port;
		this.uploaded = builder.uploaded;
		this.downloaded = builder.downloaded;
		this.left = builder.left;
		this.ip = builder.ip;
		this.numWant = builder.numWant;
		this.event = builder.event;
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public PeerId getPeerId() {
		return peerId;
	}

	public short getPort() {
		return port;
	}

	public int getUploaded() {
		return uploaded;
	}

	public int getDownloaded() {
		return downloaded;
	}

	public int getLeft() {
		return left;
	}

	public Inet6Address getIp() {
		return ip;
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
		builder.peerId(PeerId.read(data));
		builder.port(data.getShort());
		builder.uploaded(data.getInt());
		builder.downloaded(data.getInt());
		builder.left(data.getInt());
		builder.ip(IOUtils.getInet6Address(data));
		builder.numWant(data.getInt());
		builder.event(AnnounceEvent.byValue(data.get()));
		return builder.build();
	}

	public static class Builder {

		private InfoHash infoHash;
		private PeerId peerId;
		private short port;
		private int uploaded;
		private int downloaded;
		private int left;
		private Inet6Address ip;
		private int numWant;
		private AnnounceEvent event;

		public Builder infoHash(InfoHash infoHash) {
			this.infoHash = infoHash;
			return this;
		}

		public Builder peerId(PeerId peerId) {
			this.peerId = peerId;
			return this;
		}

		public Builder port(short port) {
			this.port = port;
			return this;
		}

		public Builder uploaded(int uploaded) {
			this.uploaded = uploaded;
			return this;
		}

		public Builder downloaded(int downloaded) {
			this.downloaded = downloaded;
			return this;
		}

		public Builder left(int left) {
			this.left = left;
			return this;
		}

		public Builder ip(Inet6Address ip) {
			this.ip = ip;
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
