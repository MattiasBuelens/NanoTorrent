CONTIKI_PROJECT = demo
all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki
TARGET = avr-zigduino

PROJECT_SOURCEFILES += nanotorrent.c retry.c swarm.c peer.c piece.c piece-select.c torrent.c bitset.c

PROJECTDIRS += crypto
PROJECT_SOURCEFILES += crypto.c sha1.c

PROJECTDIRS += pack
PROJECT_SOURCEFILES += pack-basic.c pack-peer.c pack-swarm.c pack-torrent.c

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
