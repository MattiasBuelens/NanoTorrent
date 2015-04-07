/**
 * \file
 *         NanoTorrent torrent descriptor reader.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include <argp.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "torrent.h"
#include "pack.h"

const char *argp_program_version = "nanoread 1.0";

// Program documentation
static char doc[] = "Read NanoTorrent torrent descriptor"
		" from SOURCE.";

// A description of the arguments we accept
static char args_doc[] = "SOURCE";

// Used by main to communicate with parse_opt
struct arguments {
	char *src_file;
};

// Parse a single option.
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	// Get the input argument from argp_parse, which we
	// know is a pointer to our arguments structure.
	struct arguments *arguments = state->input;

	switch (key) {
	case ARGP_KEY_ARG:
		switch (state->arg_num) {
		case 0:
			arguments->src_file = arg;
			break;
		default:
			// Too many arguments
			argp_usage(state);
		}
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1)
			// Not enough arguments
			argp_usage(state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

// Our argp parser
static struct argp argp = { 0, parse_opt, args_doc, doc };

#define NANOGEN_BUFFER_SIZE (1024)

int main(int argc, char **argv) {
	struct arguments arguments;

	uint8_t buffer[NANOGEN_BUFFER_SIZE];

	// Parse our arguments; every option seen by parse_opt will
	// be reflected in arguments
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	// Open source
	int src_file = open(arguments.src_file, O_RDONLY);
	if (src_file < 0) {
		ERROR("Could not open source file");
		return 0;
	}

	// Read into buffer
	int buffer_len = read(src_file, buffer, sizeof(buffer));
	if (buffer_len < 0) {
		ERROR("Could not read source file");
		goto exit_src;
	}

	// Unpack torrent descriptor
	nanotorrent_torrent_desc_t desc;
	const uint8_t *desc_cur = buffer;
	nanotorrent_unpack_torrent_desc(&desc_cur, &desc);
	size_t desc_len = desc_cur - buffer;
	if (buffer_len != desc_len) {
		ERROR("Malformed source file");
		goto exit_src;
	}

	// Calculate torrent info hash
	sha1_digest_t info_hash;
	nanotorrent_torrent_info_hash(&desc.info, &info_hash);
	char info_hash_string[2 * SHA1HashSize + 1];
	sha1_write(info_hash_string, &info_hash);

	// Print information
	printf("Info hash:\t%s\n", info_hash_string);
	printf("Tracker:\t[");
	PRINT6ADDR(&desc.tracker_ip);
	printf("]:%u\n", desc.tracker_port);
	printf("File size:\t%u B\n", desc.info.file_size);
	printf("Piece size:\t%u B\n", desc.info.piece_size);
	printf("Nb of pieces:\t%u\n", desc.info.num_pieces);
	printf("\n");
	printf("%2s | %s\n", "#", "Piece SHA-1");
	int i;
	for (i = 0; i < desc.info.num_pieces; i++) {
		printf("%02d | ", i);
		sha1_print(&desc.info.piece_hashes[i]);
		printf("\n");
	}

	exit_src: close(src_file);
	exit(0);
	return 0;
}
