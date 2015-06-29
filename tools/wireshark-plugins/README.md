# WireShark plugins for NanoTorrent

These Lua plugins add support for NanoTorrent protocol messages in Wireshark.

## Installation

Copy the `*.lua` files in this directory to [your Wireshark plugins directory](https://www.wireshark.org/docs/wsug_html_chunked/ChAppFilesConfigurationSection.html#AppFilesTabFolders).

* Windows: `%APPDATA%\Wireshark\plugins`
* Linux: `~/.wireshark/plugins`

## Usage

1. Configure the UDP ports in Wireshark > Edit > Preferences > Protocols
   * Peer protocol has default port 4242
   * Tracker protocol has default port 33333
2. Capture some NanoTorrent traffic in Wireshark, or load a pcap file from a Cooja simulation
3. Find a `NANOTORRENT` or `NANOTRACKER` protocol message in the packet list
   * You can also filter packets on `nanotorrent` and `nanotracker`
4. View the packet details
