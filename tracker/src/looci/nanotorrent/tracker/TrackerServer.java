package looci.nanotorrent.tracker;

import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.StandardProtocolFamily;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.DatagramChannel;

import looci.nanotorrent.tracker.protocol.AnnounceReply;
import looci.nanotorrent.tracker.protocol.AnnounceRequest;

public class TrackerServer {

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
		AnnounceReply reply = tracker.handleAnnounceRequest(request);
		// Write reply
		buffer.clear();
		reply.write(buffer);
		// Send reply
		buffer.flip();
		channel.send(buffer, remote);
	}

	public static void main(String[] args) throws Exception {
		if (args.length == 0) {
			System.out.println("Parameters: port [address]");
			System.exit(0);
			return;
		}

		int port = Integer.parseInt(args[0]);
		Inet6Address address = null;
		if (args.length >= 2) {
			address = (Inet6Address) InetAddress.getByName(args[1]);
		}

		TrackerServer server = new TrackerServer();
		server.bind(address, port);
		System.out.format("Tracker started on port %d of %s\n",
				server.getPort(), server.getAddress().toString());

		while (server.isConnected()) {
			server.receive();
		}
	}

}
