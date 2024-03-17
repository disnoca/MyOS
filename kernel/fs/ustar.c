#include <kernel/fs/ustar.h>
#include <kernel/mm/mm.h>
#include <kernel/mm/slab.h>
#include <kernel/system.h>
#include <kernel/utils.h>

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#ifdef __i386__
#include <kernel/arch/i386/drivers/ata.h>
#endif


struct kmem_cache_s* ustar_file_header_cache;

ustar_file_header_t aaa;


static bool is_valid_header_data(ustar_file_header_data_t* ufhd);
static bool is_valid_checksum(ustar_file_header_data_t* ufhd);


/* Global Functions */

ustar_archive_t* ustar_archive_init(uint64_t start_addr, uint64_t size)
{
	if (start_addr % USTAR_SECTOR_SIZE)
		return NULL;

	ustar_archive_t* ua = kmalloc(sizeof(ustar_archive_t));

	LIST_INIT(ua->headers);
	ua->start_addr = start_addr;
	ua->size = size;

	return ua;
}


size_t ustar_archive_load(ustar_archive_t* ua)
{
	if (!ustar_file_header_cache)
		ustar_file_header_cache = kmem_cache_create("ustar_file_header_cache", sizeof(ustar_file_header_t), NULL, NULL);

	uint64_t curr_addr = ua->start_addr;

	uint32_t disk_sector_size = ata_devices[0].logical_sector_size;
	uint8_t disk_sector_buf[disk_sector_size];

	bool unknown_size = !ua->size;

	size_t i;
	for (i = 0; unknown_size || curr_addr < ua->start_addr + ua->size; i++)
	{
		ASSERT(ata_read(0, disk_sector_buf, curr_addr, disk_sector_size));

		/* Check if the read data corresponds to a header */
		if (!is_valid_header_data((ustar_file_header_data_t*) disk_sector_buf))
			break;

		/* Allocate the header and add it to the headers list */
		ustar_file_header_t* curr_header = kmem_cache_alloc(ustar_file_header_cache);
		memcpy(&curr_header->data, disk_sector_buf, sizeof(ustar_file_header_data_t));
		curr_header->file_addr = curr_addr + USTAR_SECTOR_SIZE;
		list_add_last(&ua->headers, &curr_header->list);

		/* Update loop variables and size if necessary */
		uint64_t file_size = USTAR_HEADER_GET_FILE_SIZE(curr_header);
		curr_addr += USTAR_SECTOR_SIZE + ROUND_UP(file_size, USTAR_SECTOR_SIZE);
		if (unknown_size)
			ua->size += USTAR_SECTOR_SIZE + ROUND_UP(file_size, USTAR_SECTOR_SIZE);
	}

	return i;
}


void ustar_archive_unload(ustar_archive_t* ua)
{
	void free_header_fe(list_t* entry) {
		ASSERT(list_remove(&ua->headers, entry));
		kmem_cache_free(ustar_file_header_cache, entry);
	}

	list_for_each(&ua->headers, free_header_fe);
}


bool ustar_file_read(ustar_file_header_t* header, void* buf)
{
	return ata_read(0, buf, header->file_addr, USTAR_HEADER_GET_FILE_SIZE(header));
}


uint64_t oct2bin(char *str, size_t size) {
	uint64_t n = 0;
	unsigned char* c = (unsigned char*) str;
	while (size-- > 0) {
		n *= 8;
		n += *c - '0';
		c++;
	}
	return n;
}


/* Helper Functions */

static bool is_valid_header_data(ustar_file_header_data_t* ufhd)
{
	return ((!memcmp(ufhd->magic, TMAGIC, TMAGLEN) && !memcmp(ufhd->version, TVERSION, TVERSLEN)) ||
		   		!memcmp(ufhd->magic, OLDGNU_MAGIC, TMAGLEN + TVERSLEN)) &&
		   is_valid_checksum(ufhd);
}

static bool is_valid_checksum(ustar_file_header_data_t* ufhd)
{
	unsigned char* data = (unsigned char*) ufhd;
	unsigned int uchecksum = 0;
	unsigned int schecksum = 0;

	for (int i = 0; i < 148; i++) {
		uchecksum += data[i];
		schecksum += (signed char) data[i];
	}

	uchecksum += ((unsigned char) ' ') * 8;
	schecksum += ((signed char) ' ') * 8;

	for (int i = 156; i < 500; i++) {
		uchecksum += data[i];
		schecksum += (signed char) data[i];
	}

	unsigned int header_checksum = oct2bin(ufhd->chksum, 6);

	return uchecksum == header_checksum || schecksum == header_checksum;
}
