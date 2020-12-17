CC         = gcc
LD         = ld
BUILDDIR   = build
KERNEL_HDD = $(BUILDDIR)/disk.hdd
KERNEL_ELF = kernel/myos.kernel

BUILDSCRIPT = ./build.sh
CLEANSCRIPT = ./clean.sh

CFLAGS = -O2 -pipe -Wall -Wextra

CHARDFLAGS := $(CFLAGS)               \
	-std=gnu99                     \
	-masm=intel                    \
	-fno-pic                       \
	-mno-sse                       \
	-mno-sse2                      \
	-mno-mmx                       \
	-mno-80387                     \
	-mno-red-zone                  \
	-mcmodel=kernel                \
	-ffreestanding                 \
	-fno-stack-protector           \
	-Isrc/                         \

LSCRIPT = src/linker.ld
LDHARDFLAGS := $(LDFLAGS)        \
	-static                   \
	-nostdlib                 \
	-no-pie                   \
	-z max-page-size=0x1000   \
	-T $(LSCRIPT)

.PHONY: run clean all
.DEFAULT_GOAL = $(KERNEL_HDD)

all: run clean

disk: $(KERNEL_HDD)
run: $(KERNEL_HDD)
	qemu-system-x86_64 -m 2G -hda $(KERNEL_HDD)

$(KERNEL_ELF):
	$(BUILDSCRIPT)

limine/limine-install:
	$(MAKE) -C limine limine-install

$(KERNEL_HDD): limine/limine-install $(KERNEL_ELF)
	-mkdir build
	rm -f $(KERNEL_HDD)
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD)
	parted -s $(KERNEL_HDD) mklabel msdos
	parted -s $(KERNEL_HDD) mkpart primary 1 100%
	echfs-utils -m -p0 $(KERNEL_HDD) quick-format 32768
	echfs-utils -m -p0 $(KERNEL_HDD) import $(KERNEL_ELF) kernel.elf
	echfs-utils -m -p0 $(KERNEL_HDD) import limine.cfg limine.cfg
	limine/limine-install limine/limine.bin $(KERNEL_HDD)

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(SYSROOT)
	$(CLEANSCRIPT)
