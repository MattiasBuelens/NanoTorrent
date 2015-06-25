/**
 * \file
 *         NanoTorrent static file system.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "contiki-net.h"

#if defined(__AVR__)
/* When using non-ram storage fs-data.c must be generated with the FS_ATTR, eg
 *        ../../tools/makefsdata -A FS_ATTR
 */
#include <avr/pgmspace.h>
#define FS_ATTR PROGMEM
#define nanotorrent_fs_cpy                 memcpy_P
#define nanotorrent_fs_strcmp              strcmp_P
#define nanotorrent_fs_strchr              strchr_P
#define nanotorrent_fs_getchar(x)  pgm_read_byte(x)
#else
#define FS_ATTR
#define nanotorrent_fs_cpy                   memcpy
#define nanotorrent_fs_strcmp                strcmp
#define nanotorrent_fs_strchr                strchr
#define nanotorrent_fs_getchar(x)      (*(char *)x)
#endif

struct httpd_fsdata_file {
	const struct httpd_fsdata_file *next;
	const char *name;
	const char *data;
	const int len;
};

struct httpd_fsdata_file_noconst {
	struct httpd_fsdata_file *next;
	char *name;
	char *data;
	int len;
};

extern const struct httpd_fsdata_file *nanotorrent_fs_root;

struct httpd_fsdata_file *nanotorrent_fs_next(
		const struct httpd_fsdata_file *file);
size_t nanotorrent_fs_read(const struct httpd_fsdata_file *file,
		uint8_t *buffer, size_t buffer_len);
