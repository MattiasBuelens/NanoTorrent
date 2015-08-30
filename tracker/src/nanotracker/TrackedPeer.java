package nanotracker;

import java.util.Date;

import nanotracker.protocol.PeerInfo;

public class TrackedPeer {

	private final TrackedTorrent torrent;
	private final PeerInfo peerInfo;
	private final Date joinTime;

	private PeerState state;
	private Date lastAnnounceTime;

	public TrackedPeer(TrackedTorrent torrent, PeerInfo peerInfo, Date joinTime) {
		this.torrent = torrent;
		this.peerInfo = peerInfo;
		this.joinTime = joinTime;
		this.state = PeerState.UNKNOWN;
		this.lastAnnounceTime = joinTime;
	}

	public TrackedTorrent getTorrent() {
		return torrent;
	}

	public PeerInfo getPeerInfo() {
		return peerInfo;
	}

	public Date getJoinTime() {
		return joinTime;
	}

	public PeerState getState() {
		return state;
	}

	public void setState(PeerState state) {
		this.state = state;
	}

	public Date getLastAnnounceTime() {
		return lastAnnounceTime;
	}

	public void setLastAnnounceTime(Date lastAnnounceTime) {
		this.lastAnnounceTime = lastAnnounceTime;
	}

}
