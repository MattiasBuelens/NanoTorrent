package looci.nanotorrent.tracker;

import java.util.Date;

import looci.nanotorrent.tracker.protocol.PeerInfo;

public class TrackedPeer {

	private final PeerInfo peerInfo;
	private final Date joinTime;

	private PeerState state;
	private Date lastAnnounceTime;

	public TrackedPeer(PeerInfo peerInfo, Date joinTime) {
		this.peerInfo = peerInfo;
		this.joinTime = joinTime;
		this.state = PeerState.UNKNOWN;
		this.lastAnnounceTime = joinTime;
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
