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
		"Output file name.\nDefaults to '<infohash>.nanotorrent'" },
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
	char *src_file;
	char *tracker_ip;
	int tracker_port;
	char *dest_file;
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

#define NANOGEN_BUFFER_SIZE (1024)

int main(int argc, char **argv) {
	struct arguments arguments;

	uint8_t buffer[NANOGEN_BUFFER_SIZE];
	uint8_t *cur;

	// Default arguments
	arguments.piece_size = NANOGEN_DEFAULT_PIECE_SIZE;

	// Parse our arguments; every option seen by parse_opt will
	// be reflected in arguments
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	// Open source
	int src_file = open(arguments.src_file, O_RDONLY);
	if (src_file < 0) {
		printf("Could not open source file");
		return 0;
	}

	// Start creating torrent descriptor
	nanotorrent_torrent_desc_t desc;
	uiplib_ip6addrconv(arguments.tracker_ip, &desc.tracker_ip);
	desc.tracker_port = arguments.tracker_port;
	desc.info.piece_size = arguments.piece_size;

	// Get file size of source
	desc.info.file_size = lseek(src_file, 0l, SEEK_END);
	lseek(src_file, 0l, SEEK_SET);

	// Calculate piece hashes
	sha1_context_t piece_ctxt;
	uint8_t piece_index = 0;
	int buffer_len = 0;
	do {
		sha1_init(&piece_ctxt);
		uint16_t remaining = desc.info.piece_size;
		while (remaining > 0) {
			buffer_len = read(src_file, buffer, MIN(remaining, sizeof(buffer)));
			if (buffer_len == 0) {
				// At EOF
				break;
			}
			if (buffer_len < 0) {
				ERROR("Could not read piece %u", piece_index);
				goto exit_src;
			}
			if (!sha1_add(&piece_ctxt, buffer, buffer_len)) {
				ERROR("Could not digest piece %u", piece_index);
				goto exit_src;
			}
			remaining -= buffer_len;
		}
		if (!sha1_result(&piece_ctxt, &desc.info.piece_hashes[piece_index])) {
			ERROR("Could not calculate digest of piece %u", piece_index);
			goto exit_src;
		}
		piece_index++;
	} while (buffer_len != 0 /* EOF */);

	desc.info.num_pieces = piece_index;
	close(src_file);

	// Pack descriptor
	cur = buffer;
	nanotorrent_pack_torrent_desc(&cur, &desc);
	size_t desc_len = cur - buffer;

	// Write descriptor to temporary output file
	char out_name[64];
	strncpy(out_name, "nanogenXXXXXX", 64);
	int out_file = mkstemp(out_name);
	if (out_file < 0) {
		ERROR("Could not open output file");
		return 0;
	}
	if (write(out_file, buffer, desc_len) != desc_len) {
		ERROR("Error while writing to output file");
		return 0;
	}

	// Move to destination
	char *dest_name = arguments.dest_file;
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

	goto exit_out;

	exit_src: close(src_file);
	exit(0);
	return 0;

	exit_out: close(out_file);
	exit(0);
	return 0;
}
