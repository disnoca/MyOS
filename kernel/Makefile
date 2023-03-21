CFLAGS?=-masm=intel -O2 -g -ffreestanding -Wall -Wextra
AS?=-masm=intel -f elf
CPPFLAGS?=
LDFLAGS?=
LIBS?=-nostdlib -lk -lgcc

ARCHDIR := ./arch/i386

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
ASFLAGS:=$(ASFLAGS) $(KERNEL_ARCH_ASFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

C_SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
AS_SOURCES := $(shell find $(SOURCEDIR) -name '*.S')

ALL_OBJS := $(C_SOURCES:.c=.o) $(AS_SOURCES:.S=.o)
CRT_OBJS := $(ARCHDIR)/crti.o $(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o $(ARCHDIR)/crtn.o
KERNEL_OBJS:= $(filter-out $(CRT_OBJS), $(ALL_OBJS))

OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S

all: myos.kernel

myos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot myos.kernel

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

.S.o:
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f myos.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: myos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp myos.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
