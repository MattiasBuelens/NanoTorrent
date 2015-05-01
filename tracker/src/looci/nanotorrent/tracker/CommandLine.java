package looci.nanotorrent.tracker;

import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.nio.channels.ClosedChannelException;
import java.util.Collection;
import java.util.Scanner;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;

public class CommandLine {

	/**
	 * Maximum age of a peer's last received announce.
	 */
	public static final long MAX_PEER_AGE = 5;
	public static final TimeUnit MAX_PEER_AGE_UNIT = TimeUnit.MINUTES;

	/**
	 * Time between two peer purges.
	 */
	public static final long PURGE_PERIOD = 2;
	public static final TimeUnit PURGE_PERIOD_UNIT = TimeUnit.MINUTES;

	private final Inet6Address address;
	private final int port;

	private final Tracker tracker;
	private final TrackerServer server;

	private final ScheduledExecutorService executor;

	public CommandLine(Inet6Address address, int port) throws IOException {
		this.address = address;
		this.port = port;
		this.tracker = new Tracker();
		tracker.setLogLevel(Level.FINEST);
		this.server = new TrackerServer(tracker);
		this.executor = Executors.newScheduledThreadPool(4);
	}

	public void run() throws Exception {
		server.bind(address, port);
		System.out.format("Tracker started on port %d of %s\n",
				server.getPort(), server.getAddress().toString());

		executor.execute(new ReceiveRunner());
		executor.scheduleWithFixedDelay(new PurgeRunner(), PURGE_PERIOD,
				PURGE_PERIOD, PURGE_PERIOD_UNIT);

		@SuppressWarnings("resource")
		Scanner scanner = new Scanner(System.in);
		boolean isRunning = true;

		while (isRunning && server.isConnected()) {
			System.out.print("> ");
			String command = scanner.nextLine();
			isRunning = parseCommand(command);
		}
	}

	public void shutdown() throws IOException {
		server.close();
		executor.shutdown();
	}

	private boolean parseCommand(String command) {
		switch (command.toLowerCase()) {
		case "quit":
		case "q":
			return false;
		case "list":
		case "l":
			Collection<TrackedTorrent> torrents = server.getTracker()
					.getTorrents();
			System.out.format("Tracking %d torrent(s)\n", torrents.size());
			int i = 1;
			for (TrackedTorrent torrent : torrents) {
				System.out.format(" %d. %s\n", i, torrent.getInfoHash());
				i++;
			}
			break;
		default:
			System.out.format("Unknown command: %s\n", command);
			printCommands();
		}

		return true;
	}

	private void printCommands() {
		System.out.println("Commands:");
		System.out.println(" * list, l\tList tracked torrents");
		System.out.println(" * quit, q\tQuit");
	}

	private class ReceiveRunner implements Runnable {

		@Override
		public void run() {
			try {
				while (server.isConnected()) {
					server.receive();
				}
			} catch (ClosedChannelException e) {
				System.out.println("Tracker stopped");
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				try {
					server.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	private class PurgeRunner implements Runnable {

		@Override
		public void run() {
			server.purgePeers(MAX_PEER_AGE, MAX_PEER_AGE_UNIT);
		}

	}

	public static void main(String[] args) throws Exception {
		int port;
		if (args.length >= 1) {
			port = Integer.parseInt(args[0]);
		} else {
			@SuppressWarnings("resource")
			Scanner scanner = new Scanner(System.in);
			System.out.print("Tracker port: ");
			port = scanner.nextInt();
		}

		Inet6Address address = null;
		if (args.length >= 2) {
			address = (Inet6Address) InetAddress.getByName(args[1]);
		}

		CommandLine cmd = new CommandLine(address, port);
		try {
			cmd.run();
		} finally {
			cmd.shutdown();
		}
	}

}
