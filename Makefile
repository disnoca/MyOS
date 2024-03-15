# The project structure and this Makefile are amalgamations of OSDev's Meaty Skeleton and ToaruOS's

TOOLCHAIN = utils
SYSROOT = sysroot

ARCH = $(shell $(TOOLCHAIN)/arch.sh)
HOST = $(shell $(TOOLCHAIN)/host.sh)
KERNEL_ARCH_DIR = kernel/arch/$(ARCH)
LIBC_ARCH_DIR = libc/arch/$(ARCH)

BOOT_DIR = $(SYSROOT)/boot

LIB_DIR = $(SYSROOT)/lib
INCLUDE_DIR = $(SYSROOT)/usr/include

include build/${ARCH}.mk

AR = ${HOST}-ar
AS = nasm
CC = ${HOST}-gcc
CC += --sysroot=$(SYSROOT) -isystem=/usr/include

CFLAGS = -std=gnu11 -masm=intel -O2 -g -ffreestanding -Wall -Wextra
CFLAGS += $(KERNEL_ARCH_CFLAGS)
ASFLAGS = -f elf
ASFLAGS += $(KERNEL_ARCH_ASFLAGS)
LDFLAGS =
LDFLAGS += $(KERNEL_ARCH_LDFLAGS)

LIBS = -nostdlib -lk -lgcc
LIBS += $(KERNEL_ARCH_LIBS)
LIBK_CFLAGS = $(CFLAGS) -D__is_libk

# Add all the non architecture-specific kernel source files to the kernel object list
KERNEL_OBJS = $(patsubst %.c,%.o,$(filter-out kernel/arch/%, $(wildcard kernel/*.c kernel/*/*.c)))
#KERNEL_OBJS += $(patsubst %.S,%.o,$(filter-out kernel/arch/%, $(wildcard kernel/*.S)))

# Add the correct architecture-specific files to the kernel object list
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard ${KERNEL_ARCH_DIR}/*.c ${KERNEL_ARCH_DIR}/*/*.c))
KERNEL_OBJS += $(patsubst %.S,%.o,$(wildcard ${KERNEL_ARCH_DIR}/*.S))

CRT_OBJS = $(LIB_DIR)/crti.o $(LIB_DIR)/crtbegin.o $(LIB_DIR)/crtend.o $(LIB_DIR)/crtn.o

LIBC_OBJS = $(patsubst %.c,%.o,$(wildcard libc/*/*.c))
LIBK_OBJS = $(LIBC_OBJS:.o=.libk.o)

OBJS = \
$(KERNEL_OBJS) \
$(CRT_OBJS) \
$(LIBC_OBJS) \

LINK_LIST=\
$(LDFLAGS) \
$(LIB_DIR)/crti.o \
$(LIB_DIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(LIB_DIR)/crtend.o \
$(LIB_DIR)/crtn.o \

HEADERS = $(wildcard $(SYSROOT)/usr/include/kernel/*.h) $(wildcard $(SYSROOT)/usr/include/kernel/*/*.h) $(wildcard $(SYSROOT)/usr/include/kernel/arch/$(ARCH)/*.h)

.PHONY: all clean dirs
.SUFFIXES: .o .libk.o .a .c .S

KERNEL_TARGET = $(BOOT_DIR)/myos.kernel
LIBK_TARGET = $(LIB_DIR)/libk.a

# Not ready for libc yet.
all: dirs $(LIBK_TARGET) $(KERNEL_TARGET)

$(KERNEL_TARGET): $(KERNEL_ARCH_DIR)/linker.ld $(OBJS)
	$(CC) -T $(KERNEL_ARCH_DIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot $(KERNEL_TARGET)

kernel/%.o: kernel/%.S
	$(AS) $(ASFLAGS) $< -o $@

kernel/%.o: kernel/%.c $(HEADERS)
	$(CC) -MD -c $< -o $@ $(CFLAGS)

$(LIB_DIR)/libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)

$(LIBK_TARGET): $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

libc/%.o: libc/%.c
	$(CC) -MD -c $< -o $@ $(CFLAGS)

libc/%.libk.o: libc/%.c
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS)

libc/%.libk.o: libc/%.S
	$(AS) $(ASFLAGS) $< -o $@

$(LIB_DIR)/crtbegin.o $(LIB_DIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

$(LIB_DIR)/crt%.o: $(LIBC_ARCH_DIR)/crt%.S
	$(AS) $(ASFLAGS) $< -o $@

$(SYSROOT)/boot:
	mkdir -p $@
$(SYSROOT)/lib:
	mkdir -p $@

dirs: $(SYSROOT)/boot $(SYSROOT)/lib

clean:
	rm -rf $(BOOT_DIR)
	rm -rf $(LIB_DIR)
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
	rm -f $(LIBK_OBJS)
	rm -f $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
