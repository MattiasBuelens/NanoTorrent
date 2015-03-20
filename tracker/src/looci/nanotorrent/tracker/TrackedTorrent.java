package looci.nanotorrent.tracker;

import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import looci.nanotorrent.tracker.protocol.InfoHash;
import looci.nanotorrent.tracker.protocol.PeerInfo;

public class TrackedTorrent {

	private final InfoHash infoHash;
	private final Map<PeerInfo, TrackedPeer> peers = new HashMap<>();

	public TrackedTorrent(InfoHash infoHash) {
		this.infoHash = infoHash;
	}

	public InfoHash getInfoHash() {
		return infoHash;
	}

	public TrackedPeer getPeer(PeerInfo peerInfo) {
		return peers.get(peerInfo);
	}

	public Collection<TrackedPeer> getPeers() {
		return Collections.unmodifiableCollection(peers.values());
	}

	public TrackedPeer addPeer(PeerInfo peerInfo, Date joinTime) {
		TrackedPeer peer = new TrackedPeer(this, peerInfo, joinTime);
		peers.put(peerInfo, peer);
		return peer;
	}

	public boolean removePeer(PeerInfo peerInfo) {
		return peers.remove(peerInfo) != null;
	}

	public boolean removePeer(TrackedPeer peer) {
		if (!peer.getTorrent().equals(this)) {
			return false;
		}
		return removePeer(peer.getPeerInfo());
	}

}
