CONTIKI_PROJECT = nanotorrent
all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki
TARGET = avr-zigduino

PROJECT_SOURCEFILES += swarm.c piece.c

PROJECTDIRS += crypto
PROJECT_SOURCEFILES += crypto.c sha1.c

PROJECTDIRS += pack
PROJECT_SOURCEFILES += pack-basic.c pack-swarm.c pack-torrent.c

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
