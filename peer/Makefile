CONTIKI_PROJECT = demo
all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki

PROJECT_SOURCEFILES += nanotorrent.c retry.c swarm.c peer.c piece.c piece-select.c torrent.c bitset.c

PROJECTDIRS += crypto
PROJECT_SOURCEFILES += crypto.c sha1.c

PROJECTDIRS += pack
PROJECT_SOURCEFILES += pack-basic.c pack-peer.c pack-swarm.c pack-torrent.c

CONTIKI_WITH_IPV6 = 1

# File to seed the torrent
SEED_FILE = fs-demo/loremipsum.txt

DEFAULT_TARGET = avr-zigduino
ifeq ($(TARGET),)
  -include Makefile.target
  ifeq ($(TARGET),)
    ${info TARGET not defined, using target '$(DEFAULT_TARGET)'}
    TARGET = $(DEFAULT_TARGET)
  else
    ${info using saved target '$(TARGET)'}
  endif
endif

-include Makefile.$(TARGET)
include $(CONTIKI)/Makefile.include
