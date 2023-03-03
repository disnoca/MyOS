CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

C_SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
ASS_SOURCES := $(shell find $(SOURCEDIR) -name '*.S')
OBJECTS = $(C_SOURCES:.c=.o) $(ASS_SOURCES:.S=.o)

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
				-b boot/grub/stage2_eltorito    \
				-no-emul-boot                   \
				-boot-load-size 4               \
				-A os                           \
				-input-charset utf8             \
				-quiet                          \
				-boot-info-table                \
				-o os.iso                       \
				iso

run: os.iso
	bochs -f bochs/bochsrc.txt -q

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	find . -name "*.o" -type f -delete
	rm -rf kernel.elf os.iso
