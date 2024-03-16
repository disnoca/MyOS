#pragma once

#include <kernel/list.h>

#include <stdint.h>
#include <stddef.h>


#define USTAR_SECTOR_SIZE				512

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

#define OLDGNU_MAGIC "ustar  " /* 7 chars and a null */

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
								/* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

typedef struct ustar_file_header_data_s
{                               /* byte offset */
  	char name[100];               /*   0 */
  	char mode[8];                 /* 100 */
  	char uid[8];                  /* 108 */
  	char gid[8];                  /* 116 */
  	char size[12];                /* 124 */
  	char mtime[12];               /* 136 */
  	char chksum[8];               /* 148 */
  	char typeflag;                /* 156 */
  	char linkname[100];           /* 157 */
  	char magic[6];                /* 257 */
  	char version[2];              /* 263 */
  	char uname[32];               /* 265 */
  	char gname[32];               /* 297 */
  	char devmajor[8];             /* 329 */
  	char devminor[8];             /* 337 */
  	char prefix[155];             /* 345 */
								/* 500 */
} ustar_file_header_data_t;

typedef struct ustar_file_header_s {
	list_t list;
	ustar_file_header_data_t data;
	uint64_t file_addr;
} ustar_file_header_t;

typedef struct ustar_archive_s {
	list_t headers;
	uint64_t start_addr;
	uint64_t size;
} ustar_archive_t;



#define USTAR_HEADER_GET_FILE_SIZE(headerp) oct2bin(((ustar_file_header_t*) headerp)->data.size, 12)


/**
 * Allocates and returns an ustar archive struct.
 * 
 * The start_addr argument must be a multiple of USTAR_SECTOR_SIZE.
 * 
 * @param start_addr the archive's starting address
 * @param size the archive's size if known, otherwise 0
 * 
 * @return an allocated ustar archive struct, or NULL if start_addr isn't a multiple of USTAR_SECTOR_SIZE
*/
ustar_archive_t* ustar_archive_init(uint64_t start_addr, uint64_t size);

/**
 * Loads an ustar archive's headers into memory.
 * 
 * If the given ustar archive struct size field is 0, this method reads until the last header and updates the size field.
 * Otherwise, it only reads until size, even if more headers exist.
 * 
 * @param ua the ustar archive struct
 * 
 * @return the number of records loaded
*/
size_t ustar_archive_load(ustar_archive_t* ua);

/**
 * Unloads an ustar archive's headers from memory.
 * 
 * @param ua the ustar archive struct
*/
void ustar_archive_unload(ustar_archive_t* ua);


/**
 * Reads a file.
 * 
 * @param header the file's header
 * @param buf where to store the data
 * 
 * @return true if the read was successful, false otherwise
*/
bool ustar_file_read(ustar_file_header_t* header, void* buf);



/**
 * Converts an octadecimal string into binary.
 * 
 * @param str the string
 * @param size the string's size
 * 
 * @return the number in binary
*/
uint64_t oct2bin(char *str, size_t size);
