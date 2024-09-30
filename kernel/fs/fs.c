/**
 * File system functions.
 * 
 * @author Samuel Pires
 */

#include <kernel/fs/fs.h>
#include <kernel/fs/path_utils.h>
#include <kernel/system.h>

#include <stdio.h>
#include <string.h>


int fs_errno;


bool mounted;

void fs_init(ata_dev_t* dev)
{
	ASSERT(!mounted);
	mounted = true;

	sufs_mount(dev);
}

void* fs_open(const char* _path)
{
	char cwd[] = ROOT_DIR;
	char path[MAX_PATH_LEN];

	if (format_path(_path, cwd, path, MAX_PATH_LEN) == 0) {
		fs_errno = ENAMETOOLONG;
		return NULL;
	}

	if (!is_valid_path(path, "", MAX_FILENAME_LEN)) {
		fs_errno = ENAMETOOLONG;
		return NULL;
	}

	char* name = strrchr(path, PATH_SEPARATOR) + 1;
	if (!strcmp(name - 1, ROOT_DIR))
		name--;

	return sufs_open(path);
}

int fs_close(void* fd)
{
	return sufs_close(fd);
}

ssize_t fs_write(void* fd, void* data, uint64_t offset, size_t nbytes)
{
	if (nbytes > SSIZE_MAX) {
		fs_errno = EOVERFLOW;
		return -1;
	}

	return sufs_write(fd, data, offset, nbytes);
}

ssize_t fs_read(void* fd, void* buf, uint64_t offset, size_t nbytes)
{
	if (nbytes > SSIZE_MAX) {
		fs_errno = EOVERFLOW;
		return -1;
	}

	return sufs_read(fd, buf, offset, nbytes);
}

int fs_create(const char* _path)
{
	char cwd[] = ROOT_DIR;
	char path[MAX_PATH_LEN];

	if (format_path(_path, cwd, path, MAX_PATH_LEN) == 0) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	if (!is_valid_path(path, "", MAX_FILENAME_LEN)) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	return sufs_create(path);
}

int fs_unlink(const char* _path)
{
	char cwd[] = ROOT_DIR;
	char path[MAX_PATH_LEN];

	if (format_path(_path, cwd, path, MAX_PATH_LEN) == 0) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	if (!is_valid_path(path, "", MAX_FILENAME_LEN)) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	return sufs_unlink(path);
}

int fs_mkdir(const char* _path)
{
	char cwd[] = ROOT_DIR;
	char path[MAX_PATH_LEN];

	if (format_path(_path, cwd, path, MAX_PATH_LEN) == 0) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	if (!is_valid_path(path, "", MAX_FILENAME_LEN)) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	return sufs_mkdir(path);
}

int fs_rmdir(const char* _path)
{
	char cwd[] = ROOT_DIR;
	char path[MAX_PATH_LEN];

	if (format_path(_path, cwd, path, MAX_PATH_LEN) == 0) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	if (!is_valid_path(path, "", MAX_FILENAME_LEN)) {
		fs_errno = ENAMETOOLONG;
		return -1;
	}

	if (!strcmp(path, ROOT_DIR)) {
		fs_errno = EBUSY;
		return -1;
	}

	return sufs_rmdir(path);
}
