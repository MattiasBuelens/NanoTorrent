-- NanoTracker protocol
local proto = Proto("nanotracker", "NanoTorrent Tracker Protocol")
local default_port = 33333
local current_port = nil

-- Events
local events = {
	refresh   = 0,
	started   = 1,
	stopped   = 2,
	completed = 3
}
local event_names = {
	[0] = "refresh",
	[1] = "started",
	[2] = "stopped",
	[3] = "completed"
}

-- Fields
proto.fields.request      = ProtoField.bool("nanotracker.request", "Is request")
proto.fields.reply        = ProtoField.bool("nanotracker.reply", "Is reply")
proto.fields.info_hash    = ProtoField.bytes("nanotracker.infohash", "Info hash")
proto.fields.num_want     = ProtoField.uint8("nanotracker.numwant", "Number of peers wanted")
proto.fields.event        = ProtoField.uint8("nanotracker.event", "Event", base.DEC, event_names)
proto.fields.num_peers    = ProtoField.uint8("nanotracker.numpeers", "Number of peers")
proto.fields.peer         = ProtoField.ipv6("nanotracker.peer", "Peer address")

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

-- Announce request dissector
local function dissect_request(buffer, pinfo, tree)
	pinfo.cols.protocol = proto.name
	local subtree = tree:add(proto, buffer(), "NanoTracker Announce Request")
	local offset = 0
	-- Is request
	local is_request = subtree:add(proto.fields.request, 1)
	is_request:set_generated(true)
	-- Info hash
	subtree:add(proto.fields.info_hash, buffer(offset, 20))
	offset = offset + 20
	-- Number of peers wanted
	subtree:add(proto.fields.num_want, buffer(offset, 1))
	offset = offset + 1
	-- Event
	subtree:add(proto.fields.event, buffer(offset, 1))
	offset = offset + 1
end

-- Announce reply dissector
local function dissect_reply(buffer, pinfo, tree)
	pinfo.cols.protocol = "NANOTRACKER"
	local subtree = tree:add(proto, buffer(), "NanoTracker Announce Reply")
	local offset = 0
	-- Is reply
	local is_reply = subtree:add(proto.fields.reply, 1)
	is_reply:set_generated(true)
	-- Info hash
	subtree:add(proto.fields.info_hash, buffer(offset, 20))
	offset = offset + 20
	-- Number of peers
	local num_peers = buffer(offset, 1):uint()
	subtree:add(proto.fields.num_peers, buffer(offset, 1))
	offset = offset + 1
	-- Peers
	local peers = subtree:add(buffer(offset, 16*num_peers), "Peers")
	for i=1,num_peers do
		peers:add(proto.fields.peer, buffer(offset, 16))
		offset = offset + 16
	end
end

-- Dissector
function proto.dissector(buffer, pinfo, tree)
	if pinfo.dst_port == current_port then
		dissect_request(buffer, pinfo, tree)
	elseif pinfo.src_port == current_port then
		dissect_reply(buffer, pinfo, tree)
	end
end

-- Bootstrap
register()
