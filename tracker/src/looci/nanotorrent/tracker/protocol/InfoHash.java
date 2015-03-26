package looci.nanotorrent.tracker.protocol;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Formatter;

public class InfoHash {

	private static final int INFO_HASH_LENGTH = 20;

	private final byte[] hash = new byte[INFO_HASH_LENGTH];

	private InfoHash() {
	}

	public InfoHash(byte[] id) {
		System.arraycopy(id, 0, this.hash, 0,
				Math.min(id.length, INFO_HASH_LENGTH));
	}

	public String getHexString() {
		try (Formatter formatter = new Formatter()) {
			for (byte b : hash) {
				formatter.format("%02x", b);
			}
			return formatter.toString();
		}
	}

	public static InfoHash read(ByteBuffer data) {
		InfoHash infoHash = new InfoHash();
		data.get(infoHash.hash);
		return infoHash;
	}

	public void write(ByteBuffer data) {
		data.put(hash);
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + Arrays.hashCode(hash);
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
		InfoHash other = (InfoHash) obj;
		if (!Arrays.equals(hash, other.hash))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return getHexString();
	}

}
