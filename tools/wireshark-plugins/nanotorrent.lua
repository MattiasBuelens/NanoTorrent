-- NanoTorrent protocol
local proto = Proto("nanotorrent", "NanoTorrent Peer Protocol")
local default_port = 4242
local current_port = nil

-- Message types
local types = {
	close        = 0,
	have         = 1,
	data_request = 2,
	data_reply   = 3
}
local type_names = {
	[0] = "close",
	[1] = "have",
	[2] = "data_request",
	[3] = "data_reply"
}

-- Fields
proto.fields.type        = ProtoField.uint8("nanotorrent.type", "Message type", base.DEC, type_names)
proto.fields.info_hash   = ProtoField.bytes("nanotorrent.infohash", "Info hash")
proto.fields.have        = ProtoField.uint32("nanotorrent.have", "Have bitfield", base.HEX)
proto.fields.piece_index = ProtoField.uint8("nanotorrent.pieceindex", "Piece index")
proto.fields.data_offset = ProtoField.uint16("nanotorrent.dataoffset", "Data offset")
proto.fields.data        = ProtoField.bytes("nanotorrent.data", "Data")

-- Preferences
proto.prefs.port         = Pref.uint("Port", default_port, "UDP port number")

-- Registration
local function register()
	local dt = DissectorTable.get("udp.port")
	if current_port ~= proto.prefs.port then
		if current_port then dt:remove(current_port, proto) end
		dt:add(proto.prefs.port, proto)
		current_port = proto.prefs.port
	end
end

function proto.init()
	register()
end

-- Dissector
function proto.dissector(buffer, pinfo, tree)
	pinfo.cols.protocol = proto.name
	local subtree = tree:add(proto, buffer(), "NanoTorrent Peer Message")
	local offset = 0
	-- Message type
	local msg_type = buffer(offset, 1):uint()
	subtree:add(proto.fields.type, buffer(offset, 1))
	offset = offset + 1
	-- Info hash
	subtree:add(proto.fields.info_hash, buffer(offset, 20))
	offset = offset + 20
	-- Have bitfield
	subtree:add(proto.fields.have, buffer(offset, 4))
	offset = offset + 4
	if msg_type == types.data_request or msg_type == types.data_reply then
		-- Piece index
		subtree:add(proto.fields.piece_index, buffer(offset, 1))
		offset = offset + 1
		-- Data offset
		subtree:add(proto.fields.data_offset, buffer(offset, 2))
		offset = offset + 2
	end
	if msg_type == types.data_reply then
		-- Data
		subtree:add(proto.fields.data, buffer(offset))
	end
end

-- Bootstrap
register()
