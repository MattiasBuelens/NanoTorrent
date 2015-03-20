package looci.nanotorrent.tracker.protocol;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class AnnounceReply {

	private final InfoHash infoHash;
	private final List<PeerInfo> peers;

	protected AnnounceReply(Builder builder) {
		this.infoHash = builder.infoHash;
		this.peers = new ArrayList<PeerInfo>(builder.peers);
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public List<PeerInfo> getPeers() {
		return Collections.unmodifiableList(peers);
	}

	public void write(ByteBuffer data) {
		infoHash.write(data);
		data.put((byte) peers.size());
		for (PeerInfo peer : peers) {
			peer.write(data);
		}
	}

	public static class Builder {

		private InfoHash infoHash;
		private List<PeerInfo> peers = new ArrayList<PeerInfo>();

		public Builder infoHash(InfoHash infoHash) {
			this.infoHash = infoHash;
			return this;
		}

		public void addPeer(PeerInfo peer) {
			this.peers.add(peer);
		}

		public AnnounceReply build() {
			return new AnnounceReply(this);
		}

	}

}
