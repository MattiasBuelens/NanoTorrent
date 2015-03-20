package looci.nanotorrent.tracker.protocol;

import java.net.Inet6Address;
import java.nio.ByteBuffer;

public final class PeerInfo {

	private final Inet6Address address;
	private final short port;

	public PeerInfo(Inet6Address address, short port) {
		this.address = address;
		this.port = port;
	}

	public Inet6Address getAddress() {
		return address;
	}

	public short getPort() {
		return port;
	}

	public void write(ByteBuffer data) {
		IOUtils.putInet6Address(data, address);
		data.putShort(port);
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((address == null) ? 0 : address.hashCode());
		result = prime * result + port;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		PeerInfo other = (PeerInfo) obj;
		if (address == null) {
			if (other.address != null)
				return false;
		} else if (!address.equals(other.address))
			return false;
		if (port != other.port)
			return false;
		return true;
	}

}
