#ifndef TSS
#define TSS

#include "types.h"

struct tss {
    uint32_t   link;
    uint32_t   link_res;

    uint64_t   esp0;

    uint32_t   ss0;
    uint32_t   ss0_res;

    uint64_t   esp1;
	
    uint32_t   ss1;
    uint32_t   ss1_res;

    uint64_t   esp2;
    uint32_t   ss2;
    uint32_t   ss2_res;

    uint64_t   cr3;

    uint64_t   eip;

    uint64_t   eflags;

    uint64_t   eax;
    uint64_t   ecx;
    uint64_t   edx;
    uint64_t   ebx;

    uint64_t   esp;
    uint64_t   ebp;

    uint64_t   esi;
    uint64_t   edi;

    uint32_t   es;
    uint32_t   es_res;

    uint32_t   cs;
    uint32_t   cs_res;

    uint32_t   ss;
    uint32_t   ss_res;

    uint32_t   ds;
    uint32_t   ds_res;

    uint32_t   fs;
    uint32_t   fs_res;

    uint32_t   gs;
    uint32_t   gs_res;

    uint32_t   ldt;
    uint32_t   ldt_res;

    uint32_t   trap;
    uint32_t   iomap;

	uint64_t   ssp;

} __attribute__((packed));

extern volatile struct tss tss;

#endif