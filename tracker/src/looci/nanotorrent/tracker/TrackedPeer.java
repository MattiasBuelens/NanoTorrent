package looci.nanotorrent.tracker;

import java.net.Inet6Address;

import looci.nanotorrent.tracker.protocol.PeerId;

public class TrackedPeer {

	private final PeerId id;
	private final Inet6Address address;
	private final int port;

	public TrackedPeer(PeerId peerId, Inet6Address peerAddress, int peerPort) {
		this.id = peerId;
		this.address = peerAddress;
		this.port = peerPort;
	}

}
