package looci.nanotorrent.tracker;

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

}
