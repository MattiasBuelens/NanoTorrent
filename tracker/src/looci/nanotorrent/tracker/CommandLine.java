package looci.nanotorrent.tracker;

import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.nio.channels.ClosedChannelException;
import java.util.Collection;
import java.util.Scanner;
import java.util.logging.Level;

public class CommandLine {

	private final Inet6Address address;
	private final int port;

	private final Tracker tracker;
	private final TrackerServer server;

	public CommandLine(Inet6Address address, int port) throws IOException {
		this.address = address;
		this.port = port;
		this.tracker = new Tracker();
		tracker.setLogLevel(Level.FINEST);
		this.server = new TrackerServer(tracker);
	}

	public void run() throws Exception {
		server.bind(address, port);
		System.out.format("Tracker started on port %d of %s\n",
				server.getPort(), server.getAddress().toString());

		Thread runner = new Thread(new ReceiveRunner());
		runner.start();

		@SuppressWarnings("resource")
		Scanner scanner = new Scanner(System.in);
		boolean isRunning = true;

		while (isRunning) {
			System.out.print("> ");
			String command = scanner.nextLine();
			isRunning = parseCommand(command);
		}

		server.close();
		runner.join();
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
			}
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
		cmd.run();
	}

}
