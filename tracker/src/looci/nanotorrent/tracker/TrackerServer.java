package looci.nanotorrent.tracker;

import java.io.Closeable;
import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetSocketAddress;
import java.net.StandardProtocolFamily;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.DatagramChannel;
import java.util.concurrent.TimeUnit;

import looci.nanotorrent.tracker.protocol.AnnounceReply;
import looci.nanotorrent.tracker.protocol.AnnounceRequest;
import looci.nanotorrent.tracker.protocol.PeerInfo;

public class TrackerServer implements Closeable {

	private static final int BUFFER_SIZE = 1024;

	protected final Tracker tracker;
	protected final DatagramChannel channel;

	private final ByteBuffer buffer;
	private boolean bound = false;

	public TrackerServer(Tracker tracker) throws IOException {
		this.tracker = tracker;
		this.channel = DatagramChannel.open(StandardProtocolFamily.INET6);
		this.channel.configureBlocking(true);
		this.buffer = ByteBuffer.allocate(BUFFER_SIZE);
		this.buffer.order(ByteOrder.BIG_ENDIAN);
	}

	public TrackerServer() throws IOException {
		this(new Tracker());
	}

	public Tracker getTracker() {
		return tracker;
	}

	public boolean isConnected() {
		return bound && channel.isOpen();
	}

	protected InetSocketAddress getSocketAddress() {
		if (!isConnected()) {
			throw new IllegalStateException("Tracker not connected");
		}
		try {
			return (InetSocketAddress) channel.getLocalAddress();
		} catch (IOException e) {
			throw new IllegalStateException("Tracker has no local address", e);
		}
	}

	public Inet6Address getAddress() {
		return (Inet6Address) getSocketAddress().getAddress();
	}

	public int getPort() {
		return getSocketAddress().getPort();
	}

	public void bind(Inet6Address address, int port) throws IOException {
		if (bound) {
			throw new IllegalStateException("Tracker was previously bound");
		}
		channel.bind(new InetSocketAddress(address, port));
		bound = true;
	}

	public void bind(int port) throws IOException {
		bind(null, port);
	}

	@Override
	public void close() throws IOException {
		channel.close();
	}

	public void receive() throws IOException {
		if (!isConnected()) {
			throw new IllegalStateException("Tracker not connected");
		}
		// Receive request
		buffer.clear();
		InetSocketAddress remote = (InetSocketAddress) channel.receive(buffer);
		// Parse request
		buffer.flip();
		AnnounceRequest request = AnnounceRequest.read(buffer);
		// Handle request
		PeerInfo peerInfo = new PeerInfo((Inet6Address) remote.getAddress());
		AnnounceReply reply = tracker.handleAnnounceRequest(request, peerInfo);
		// Write reply
		buffer.clear();
		reply.write(buffer);
		// Send reply
		buffer.flip();
		channel.send(buffer, remote);
	}

	public void purgePeers(long maxPeerAge, TimeUnit ageUnit) {
		getTracker().purgePeers(maxPeerAge, ageUnit);
	}

}
