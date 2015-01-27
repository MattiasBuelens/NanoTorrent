package looci.nanotorrent.tracker.protocol;

import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

public class IOUtils {

	public static Inet6Address getInet6Address(ByteBuffer data) {
		byte[] ipBytes = new byte[16];
		data.get(ipBytes);
		try {
			return (Inet6Address) InetAddress.getByAddress(ipBytes);
		} catch (UnknownHostException e) {
			throw new AssertionError("IPv6 address must be 16 bytes long", e);
		}
	}

	public static void putInet6Address(ByteBuffer data, Inet6Address address) {
		byte[] ipBytes = address.getAddress();
		if (ipBytes.length != 16) {
			throw new AssertionError("IPv6 address must be 16 bytes long", null);
		}
		data.put(ipBytes);
	}

}
