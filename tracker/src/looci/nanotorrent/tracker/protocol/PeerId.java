package looci.nanotorrent.tracker.protocol;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class PeerId {

	private static final int PEER_ID_LENGTH = 20;

	private final byte[] id = new byte[PEER_ID_LENGTH];

	private PeerId() {
	}

	public PeerId(byte[] id) {
		System.arraycopy(id, 0, this.id, 0, Math.min(id.length, PEER_ID_LENGTH));
	}

	public static PeerId read(ByteBuffer data) {
		PeerId peerId = new PeerId();
		data.get(peerId.id);
		return peerId;
	}

	public void write(ByteBuffer data) {
		data.put(id);
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + Arrays.hashCode(id);
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
		PeerId other = (PeerId) obj;
		if (!Arrays.equals(id, other.id))
			return false;
		return true;
	}

}
