#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../sysroot/usr/include/kernel/fs/sufs.h"
#include "../../sysroot/usr/include/kernel/fs/fs.h"
#include "../../sysroot/usr/include/kernel/utils.h"


#define BLOCK_SIZE_DEFAULT		(1 << 10)

#define INODE_DENSITY_DEFAULT	((1 << 10) * 16)

#define SECTOR_SIZE 512


FILE* disk_fp;
size_t disk_size;
uint8_t* block_buf;
struct sufs_superblock sb;


static void write_superblock();
static void write_maps();
static void write_inodes();
static void write_rootdir();


int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: %s <disk.img>\n", argv[0]);
		return 1;
	}

	disk_fp = fopen(argv[1], "r+b");
	fseek(disk_fp, 0, SEEK_END);
	disk_size = ftell(disk_fp);
	block_buf = aligned_alloc(sizeof(struct sufs_dinode), BLOCK_SIZE_DEFAULT);

	write_superblock();
	write_maps();
	write_inodes();
	write_rootdir();

	free(block_buf);
	fclose(disk_fp);
	return 0;
}


static void write_superblock()
{
	sb.sb_block_size = BLOCK_SIZE_DEFAULT;
	if (SECTOR_SIZE > sb.sb_block_size) {
		fprintf(stderr, "Block size cannot be smaller than sector size\n");
		exit(1);
	}

	sb.sb_block_count = disk_size / sb.sb_block_size;

	sb.sb_secpb = sb.sb_block_size / SECTOR_SIZE;
	sb.sb_nindir = sb.sb_block_size / sizeof(sufs_daddr_t);
	sb.sb_inopb = sb.sb_block_size / sizeof(struct sufs_dinode);
	sb.sb_mapentpb = sb.sb_block_size * 8;
	sb.sb_dentpb = sb.sb_block_size / sizeof(struct sufs_dentry);

	sb.sb_inode_map_boff = DIV_CEIL(SUFS_BOOTBLOCK_SIZE + sizeof(struct sufs_superblock) * 2, sb.sb_block_size);
	sb.sb_inode_count = ALIGN_UP(disk_size / INODE_DENSITY_DEFAULT, sb.sb_block_size);
	sb.sb_iblock_count = DIV_CEIL(sb.sb_inode_count * sizeof(struct sufs_dinode), sb.sb_block_size);
	sb.sb_inode_map_bsize = DIV_CEIL(DIV_CEIL(sb.sb_inode_count, 8), sb.sb_block_size);
	// TODO: free size could be used instead of fs size to calculate inode count

	sb.sb_dblock_map_boff = sb.sb_inode_map_boff + sb.sb_inode_map_bsize;
	sb.sb_dblock_count = sb.sb_block_count - sb.sb_dblock_map_boff - sb.sb_iblock_count;
	sb.sb_dblock_map_bsize = DIV_CEIL(DIV_CEIL(sb.sb_dblock_count, 8), sb.sb_block_size);
	sb.sb_dblock_count -= sb.sb_dblock_map_bsize;
	// TODO: adjustments could be made here to regain space occupied by the just discarded data blocks through the map

	sb.sb_inodes_boff = sb.sb_dblock_map_boff + sb.sb_dblock_map_bsize;
	sb.sb_dblocks_boff = sb.sb_inodes_boff + sb.sb_iblock_count;

	if (sb.sb_dblocks_boff >= sb.sb_block_count || sb.sb_inode_count >= sb.sb_dblock_count) {
		fprintf(stderr, "Disk is too small\n");
		exit(1);
	}
	
	sb.sb_roodir_inum = 1;
	sb.sb_free_inode_count = sb.sb_inode_count - 1;	// root dir
	sb.sb_free_dblock_count = sb.sb_dblock_count - 1;

	sb.sb_maxfilesize = SUFS_NDADDR;
	for (int i = 0; i < SUFS_NIADDR; i++) {
		uint32_t extra = sb.sb_nindir;
		for (int j = 0; j < i; j++)
			extra *= sb.sb_nindir;
		sb.sb_maxfilesize += extra;
	}
	sb.sb_maxfilesize *= sb.sb_block_size;

	sb.sb_magic = SUFS_MAGIC;
	sb.sb_time = time(NULL);

	fseek(disk_fp, SUFS_SUPERBLOCK_OFFSET, SEEK_SET);
	fwrite(&sb, sizeof(sb), 1, disk_fp);
}

static void write_maps()
{
	memset(block_buf, 0, sb.sb_block_size);

	fseek(disk_fp, sb.sb_block_size * sb.sb_inode_map_boff, SEEK_SET);
	block_buf[0] = 0x03;	// dblock/inode 0 and root dir's
	fwrite(block_buf, sb.sb_block_size, 1, disk_fp);
	block_buf[0] = 0x00;
	for (uint32_t i = 1; i < sb.sb_inode_map_bsize; i++)
		fwrite(block_buf, sb.sb_block_size, 1, disk_fp);

	fseek(disk_fp, sb.sb_block_size * sb.sb_dblock_map_boff, SEEK_SET);
	block_buf[0] = 0x03;
	fwrite(block_buf, sb.sb_block_size, 1, disk_fp);
	block_buf[0] = 0x00;
	for (uint32_t i = 1; i < sb.sb_dblock_map_bsize; i++)
		fwrite(block_buf, sb.sb_block_size, 1, disk_fp);
}

static void write_inodes()
{
	block_buf[0] = 0;
	struct sufs_dinode* di = ((struct sufs_dinode*)block_buf + 1);

	di->di_ctime = di->di_atime = di->di_mtime = di->di_itime = sb.sb_time;
	di->di_inumber = 1;
	di->di_nblocks = 1;
	di->di_nlink = 2;
	di->di_mode = IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	di->di_db[0] = sb.sb_dblocks_boff + 1;

	fseek(disk_fp, sb.sb_block_size * sb.sb_inodes_boff, SEEK_SET);
	fwrite(block_buf, sb.sb_block_size, 1, disk_fp);
}

static void write_rootdir()
{
	memset(block_buf, 0, sb.sb_block_size);

	struct sufs_dentry* de = (struct sufs_dentry*)block_buf;
	de->de_inum = 1;
	strcpy(de->de_name, ".");
	de++;
	de->de_inum = 1;
	strcpy(de->de_name, "..");

	fseek(disk_fp, sb.sb_block_size * (sb.sb_dblocks_boff + 1), SEEK_SET);
	fwrite(block_buf, sb.sb_block_size, 1, disk_fp);
}
