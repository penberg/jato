/*
 * Copyright (C) 2011  Pekka Enberg
 *
 * This file is released under the GPL version 2 with the following
 * clarification and special exception:
 *
 *     Linking this library statically or dynamically with other modules is
 *     making a combined work based on this library. Thus, the terms and
 *     conditions of the GNU General Public License cover the whole
 *     combination.
 *
 *     As a special exception, the copyright holders of this library give you
 *     permission to link this library with independent modules to produce an
 *     executable, regardless of the license terms of these independent
 *     modules, and to copy and distribute the resulting executable under terms
 *     of your choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module. An
 *     independent module is a module which is not derived from or based on
 *     this library. If you modify this library, you may extend this exception
 *     to your version of the library, but you are not obligated to do so. If
 *     you do not wish to do so, delete this exception statement from your
 *     version.
 *
 * Please refer to the file LICENSE for details.
 */

#include "dalvik/dex-file.h"

#include "vm/die.h"
#include "vm/gc.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

uint8_t DEX_FILE_MAGIC[8] = { 0x64, 0x65, 0x78, 0x0a, 0x30, 0x33, 0x35, 0x00 };

static int dex_parse_header(struct dex_file *dex)
{
	dex->dx_header = dex->dx_mmap;

	if (memcmp(dex->dx_header->dx_magic, DEX_FILE_MAGIC, 8))
		return warn("bad magic"), -EINVAL;

	if (dex->dx_header->dx_header_size != 0x70)
		return warn("bad header size"), -EINVAL;

	if (dex->dx_header->dx_endian_tag != DEX_ENDIAN_CONSTANT)
		return warn("bad endian"), -EINVAL;

	return 0;
}

struct dex_file *dex_open(const char *filename)
{
	struct dex_file *dex;
	struct stat st;

	dex = vm_zalloc(sizeof *dex);
	if (!dex)
		return NULL;

	dex->dx_fd = open(filename, O_RDONLY);
	if (dex->dx_fd < 0) {
		warn("open");
		goto error_free_dex;
	}

	if (fstat(dex->dx_fd, &st) < 0) {
		warn("fstat"), NULL;
		goto error_close_file;
	}

	dex->dx_size = st.st_size;

	dex->dx_mmap = mmap(NULL, dex->dx_size, PROT_READ, MAP_PRIVATE, dex->dx_fd, 0);
	if (dex->dx_mmap == MAP_FAILED) {
		warn("mmap");
		goto error_close_file;
	}

	if (dex_parse_header(dex) < 0)
		goto error_munmap_file;

	return dex;

error_munmap_file:
	munmap(dex->dx_mmap, dex->dx_size);

error_close_file:
	close(dex->dx_fd);

error_free_dex:
	vm_free(dex);

	return NULL;
}

void dex_close(struct dex_file *self)
{
	munmap(self->dx_mmap, self->dx_size);
	close(self->dx_fd);
	free(self);
}
