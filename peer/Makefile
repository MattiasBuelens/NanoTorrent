CONTIKI_PROJECT = demo
all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki

PROJECT_SOURCEFILES += nanotorrent.c retry.c swarm.c peer.c piece.c piece-select.c torrent.c bitset.c
PROJECT_SOURCEFILES += fs.c

PROJECT_SOURCEFILES += crypto.c
PROJECTDIRS += crypto/ref
PROJECT_SOURCEFILES += crypto-impl.c sha1.c

PROJECTDIRS += pack
PROJECT_SOURCEFILES += pack-basic.c pack-peer.c pack-swarm.c pack-torrent.c

CONTIKI_WITH_IPV6 = 1

# Torrent seed file
TORRENT_FILE = ./fs-demo/loremipsum.txt

# Torrent seed file name
TORRENT_NAME = $(basename $(notdir $(TORRENT_FILE)))

# Torrent descriptor file
# Must be single file in separate folder (to use with makefsdata)
TORRENT_DESC = $(dir $(TORRENT_FILE))torrent/$(TORRENT_NAME).nt

fs.c: fs-data.c

fs-data.c: $(TORRENT_DESC)
	$(CONTIKI)/tools/makefsdata -A FS_ATTR -f 16 -d $(dir $<) -o $@

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
