package looci.nanotorrent.tracker;

import java.util.HashMap;
import java.util.Map;

import looci.nanotorrent.tracker.protocol.InfoHash;

public class Tracker {

	private final Map<InfoHash, TrackedTorrent> torrents = new HashMap<>();

	public TrackedTorrent track(InfoHash infoHash) {
		TrackedTorrent torrent = new TrackedTorrent(infoHash);
		torrents.put(infoHash, torrent);
		return torrent;
	}

	public boolean untrack(InfoHash infoHash) {
		return torrents.remove(infoHash) != null;
	}

	public boolean untrack(TrackedTorrent torrent) {
		return untrack(torrent.getInfoHash());
	}

}
