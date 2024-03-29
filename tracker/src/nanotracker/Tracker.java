package nanotracker;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import nanotracker.protocol.AnnounceEvent;
import nanotracker.protocol.AnnounceReply;
import nanotracker.protocol.AnnounceRequest;
import nanotracker.protocol.InfoHash;
import nanotracker.protocol.PeerInfo;

public class Tracker {

	protected final Logger log = Logger
			.getLogger(Tracker.class.getSimpleName());

	private final Map<InfoHash, TrackedTorrent> torrents = new HashMap<>();

	public TrackedTorrent getTorrent(InfoHash infoHash) {
		return torrents.get(infoHash);
	}

	public Collection<TrackedTorrent> getTorrents() {
		return Collections.unmodifiableCollection(torrents.values());
	}

	public TrackedTorrent track(InfoHash infoHash) {
		log(Level.INFO, "Start tracking torrent %s", infoHash);
		TrackedTorrent torrent = new TrackedTorrent(infoHash);
		torrents.put(infoHash, torrent);
		return torrent;
	}

	public boolean untrack(InfoHash infoHash) {
		log(Level.INFO, "Stop tracking torrent %s", infoHash);
		return torrents.remove(infoHash) != null;
	}

	public boolean untrack(TrackedTorrent torrent) {
		return untrack(torrent.getInfoHash());
	}

	public AnnounceReply handleAnnounceRequest(AnnounceRequest request,
			PeerInfo peerInfo) {
		log(Level.FINE, "Announce request received from %s with event %s",
				peerInfo, request.getEvent());

		TrackedTorrent torrent = getTorrent(request.getInfoHash());
		if (torrent == null) {
			// TODO Okay to automatically track?
			torrent = track(request.getInfoHash());
		}

		Date now = new Date();
		AnnounceReply.Builder reply = new AnnounceReply.Builder();
		reply.infoHash(torrent.getInfoHash());

		TrackedPeer peer = torrent.getPeer(peerInfo);

		if (request.getEvent() == AnnounceEvent.STOPPED) {
			if (peer != null) {
				// Update state in case anyone has a reference to this peer
				peer.setLastAnnounceTime(now);
				updatePeerState(peer, request.getEvent());
			}
			// Stop tracking peer
			log(Level.INFO, "Stop tracking peer %s in %s", peerInfo,
					torrent.getInfoHash());
			torrent.removePeer(peerInfo);
			// Return empty reply
			return reply.build();
		}

		if (peer == null) {
			// Start tracking peer
			log(Level.INFO, "Start tracking peer %s in %s", peerInfo,
					torrent.getInfoHash());
			peer = torrent.addPeer(peerInfo, now);
		}

		// Update state
		peer.setLastAnnounceTime(now);
		updatePeerState(peer, request.getEvent());

		// Find some other peers
		List<PeerInfo> otherPeers = getOtherPeers(peer, request.getNumWant());
		reply.addPeers(otherPeers);
		log(Level.FINE, "Replied with %d other peers", otherPeers.size());

		return reply.build();
	}

	public void purgePeers(long maxAge, TimeUnit ageUnit) {
		purgePeers(ageUnit.toMillis(maxAge));
	}

	public void purgePeers(long maxAgeInMillis) {
		Date minTime = new Date(System.currentTimeMillis() - maxAgeInMillis);
		purgePeers(minTime);
	}

	public void purgePeers(Date minAnnounceTime) {
		for (TrackedTorrent torrent : getTorrents()) {
			purgePeers(torrent, minAnnounceTime);
		}
	}

	private void purgePeers(TrackedTorrent torrent, Date minAnnounceTime) {
		List<TrackedPeer> peers = new ArrayList<>(torrent.getPeers());
		for (TrackedPeer peer : peers) {
			Date lastAnnounce = peer.getLastAnnounceTime();
			if (lastAnnounce.before(minAnnounceTime)) {
				// Purge peer
				log(Level.INFO, "Purging peer %s in %s (last announce: %s)",
						peer.getPeerInfo(), torrent.getInfoHash(),
						lastAnnounce.toString());
				torrent.removePeer(peer.getPeerInfo());
			}
		}
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

	protected void log(Level level, String format, Object... args) {
		log.log(level, String.format(format, args));
	}

	public void setLogLevel(Level level) {
		log.setLevel(level);
	}

}
