/**
 * \file
 *         NanoTorrent torrent descriptor generator.
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
#include "piece.h"
#include "pack.h"

#define NANOGEN_DEFAULT_PIECE_SIZE 256

const char *argp_program_version = "nanogen 1.0";

// Program documentation
static char doc[] = "Generate NanoTorrent torrent descriptor"
		" for SOURCE"
		" tracked by TRACKERIP on port TRACKERPORT.";

// A description of the arguments we accept
static char args_doc[] = "SOURCE TRACKERIP TRACKERPORT";

// The options we understand
static struct argp_option options[] = {
/**
 * Output file
 */
{ "output", 'o', "DEST", 0,
		"Output file name\nDefaults to '<infohash>.nanotorrent'" },
/**
 * Piece size
 */
{ "piece", 'p', "SIZE", 0, "Piece size in bytes\nDefaults to "
QUOTE(NANOGEN_DEFAULT_PIECE_SIZE) "" },
/**
 * End marker
 */
{ 0 } };

// Used by main to communicate with parse_opt
struct arguments {
	const char *src_file;
	const char *tracker_ip;
	int tracker_port;
	const char *dest_file;
	int piece_size;
};

// Parse a single option.
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	// Get the input argument from argp_parse, which we
	// know is a pointer to our arguments structure.
	struct arguments *arguments = state->input;

	switch (key) {
	case 'o':
		arguments->dest_file = arg;
		break;
	case 'p':
		arguments->piece_size = atoi(arg);
		break;
	case ARGP_KEY_ARG:
		switch (state->arg_num) {
		case 0:
			arguments->src_file = arg;
			break;
		case 1:
			arguments->tracker_ip = arg;
			break;
		case 2:
			arguments->tracker_port = atoi(arg);
			break;
		default:
			// Too many arguments
			argp_usage(state);
		}
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 3)
			// Not enough arguments
			argp_usage(state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

// Our argp parser
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv) {
	struct arguments arguments = { 0 };

	// Default arguments
	arguments.piece_size = NANOGEN_DEFAULT_PIECE_SIZE;

	// Parse our arguments; every option seen by parse_opt will
	// be reflected in arguments
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	// Open source
	int src_file = open(arguments.src_file, O_RDONLY);
	if (src_file < 0) {
		printf("Could not open source file");
		return -1;
	}

	// Create torrent descriptor
	nanotorrent_torrent_desc_t desc;
	uiplib_ip6addrconv(arguments.tracker_ip, &desc.tracker_ip);
	desc.tracker_port = arguments.tracker_port;
	// Create torrent info
	if (!nanotorrent_torrent_info_create(&desc.info, src_file,
			arguments.piece_size)) {
		goto error_src;
	}

	// Done processing source
	close(src_file);

	// Pack descriptor
	uint8_t buffer[1024];
	uint8_t *desc_end = nanotorrent_pack_torrent_desc(buffer, &desc);
	size_t desc_len = desc_end - buffer;

	// Write descriptor to temporary output file
	char out_name[64];
	strncpy(out_name, "nanogenXXXXXX", 64);
	int out_file = mkstemp(out_name);
	if (out_file < 0) {
		ERROR("Could not open output file");
		goto error;
	}
	if (write(out_file, buffer, desc_len) != desc_len) {
		ERROR("Error while writing to output file");
		goto error_out;
	}

	// Move to destination
	const char *dest_name = arguments.dest_file;
	char dest_gen_name[128];
	if (dest_name == NULL) {
		// Calculate torrent info hash
		sha1_digest_t info_hash;
		nanotorrent_torrent_info_hash(&desc.info, &info_hash);
		char info_hash_string[2 * SHA1HashSize + 1];
		sha1_write(info_hash_string, &info_hash);
		// Generate destination name
		strcpy(&dest_gen_name[0], info_hash_string);
		strcpy(&dest_gen_name[2 * SHA1HashSize], ".nanotorrent");
		dest_name = dest_gen_name;
	}
	rename(out_name, dest_name);

	close(out_file);
	exit(0);
	return 0;

	error_src: close(src_file);
	goto error;

	error_out: close(out_file);
	goto error;

	error: exit(-1);
	return -1;
}
