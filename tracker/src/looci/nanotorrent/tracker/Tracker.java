package looci.nanotorrent.tracker;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import looci.nanotorrent.tracker.protocol.AnnounceEvent;
import looci.nanotorrent.tracker.protocol.AnnounceReply;
import looci.nanotorrent.tracker.protocol.AnnounceRequest;
import looci.nanotorrent.tracker.protocol.InfoHash;
import looci.nanotorrent.tracker.protocol.PeerInfo;

public class Tracker {

	private final Map<InfoHash, TrackedTorrent> torrents = new HashMap<>();

	public TrackedTorrent getTorrent(InfoHash infoHash) {
		return torrents.get(infoHash);
	}

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

	public AnnounceReply handleAnnounceRequest(AnnounceRequest request) {
		TrackedTorrent torrent = getTorrent(request.getInfoHash());
		if (torrent == null) {
			// TODO Better error handling?
			return null;
		}

		Date now = new Date();
		AnnounceReply.Builder reply = new AnnounceReply.Builder();
		reply.infoHash(torrent.getInfoHash());

		TrackedPeer peer = torrent.getPeer(request.getPeerInfo());

		if (request.getEvent() == AnnounceEvent.STOPPED) {
			if (peer != null) {
				// Update state in case anyone has a reference to this peer
				peer.setLastAnnounceTime(now);
				updatePeerState(peer, request.getEvent());
			}
			// Stop tracking peer
			torrent.removePeer(request.getPeerInfo());
			// Return empty reply
			return reply.build();
		}

		if (peer == null) {
			// Start tracking peer
			peer = torrent.addPeer(request.getPeerInfo(), now);
		}

		// Update state
		peer.setLastAnnounceTime(now);
		updatePeerState(peer, request.getEvent());

		// Find some other peers
		reply.addPeers(getOtherPeers(peer, request.getNumWant()));

		return reply.build();
	}

	private List<PeerInfo> getOtherPeers(TrackedPeer peer, int maxPeers) {
		if (maxPeers <= 0) {
			// No peers needed
			return Collections.emptyList();
		}

		List<TrackedPeer> candidates = getCandidates(peer);
		return collectPeers(peer, maxPeers, candidates);
	}

	private List<TrackedPeer> getCandidates(TrackedPeer peer) {
		// Simply shuffle all peers
		// TODO Make this more intelligent
		TrackedTorrent torrent = peer.getTorrent();
		List<TrackedPeer> candidates = new ArrayList<TrackedPeer>(
				torrent.getPeers());
		Collections.shuffle(candidates);
		return candidates;
	}

	private List<PeerInfo> collectPeers(TrackedPeer peer, int maxPeers,
			List<TrackedPeer> candidates) {
		if (maxPeers <= 0) {
			// No peers needed
			return Collections.emptyList();
		}

		List<PeerInfo> otherPeers = new ArrayList<PeerInfo>();
		int nbPeers = 0;
		for (TrackedPeer candidate : candidates) {
			if (!isValidCandidate(candidate, peer)) {
				// Ignore invalid candidate
				continue;
			}

			// Add peer
			otherPeers.add(candidate.getPeerInfo());

			nbPeers++;
			if (nbPeers == maxPeers) {
				// Got enough peers
				break;
			}
		}

		return otherPeers;
	}

	private boolean isValidCandidate(TrackedPeer candidate, TrackedPeer peer) {
		if (candidate.getState() == PeerState.STOPPED) {
			// Candidate cannot be stopped
			return false;
		}
		if (candidate.getPeerInfo().equals(peer.getPeerInfo())) {
			// Candidate cannot be same peer
			return false;
		}
		return true;
	}

	private void updatePeerState(TrackedPeer peer, AnnounceEvent event) {
		switch (event) {
		case REFRESH:
			// Retain current state
			break;
		case STARTED:
			peer.setState(PeerState.STARTED);
			break;
		case STOPPED:
			peer.setState(PeerState.STOPPED);
			break;
		case COMPLETED:
			peer.setState(PeerState.COMPLETED);
			break;
		default:
			// Unknown state
			peer.setState(PeerState.UNKNOWN);
			break;
		}
	}

}
