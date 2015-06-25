/**
 * \file
 *         NanoTorrent static file system.
 * \author
 *         Mattias Buelens <mattias.buelens@student.kuleuven.be>
 */

#include "common.h"
#include "fs.h"
#include "fs-data.c"

const struct httpd_fsdata_file *nanotorrent_fs_root = HTTPD_FS_ROOT;

struct httpd_fsdata_file *nanotorrent_fs_next(
		const struct httpd_fsdata_file *file) {
	struct httpd_fsdata_file *next;
	nanotorrent_fs_cpy(&next, &file->next, sizeof(next));
	return next;
}

size_t nanotorrent_fs_read(const struct httpd_fsdata_file *file,
		uint8_t *buffer, size_t buffer_len) {
	// Read file into RAM
	struct httpd_fsdata_file f;
	nanotorrent_fs_cpy(&f, file, sizeof(f));
	// Read file data
	size_t len = MIN(buffer_len, f.len);
	nanotorrent_fs_cpy(buffer, f.data, len);
	return len;
}
