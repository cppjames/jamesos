BUILDDIR   = build
KERNEL_HDD = $(BUILDDIR)/jamesos.hdd
KERNEL_ELF = kernel/jamesos.kernel

BUILDSCRIPT = scripts/build.sh
CLEANSCRIPT = scripts/clean.sh

CFLAGS = -O2 -pipe -Wall -Wextra

CHARDFLAGS := $(CFLAGS)               	\
	-std=gnu99                     		\
	-masm=intel                    		\
	-fno-pic                       		\
	-mno-sse                       		\
	-mno-sse2                      		\
	-mno-mmx                       		\
	-mno-80387                     		\
	-mno-red-zone                  		\
	-mcmodel=kernel                		\
	-ffreestanding                 		\
	-fno-stack-protector           		\
	-Isrc/                         		\

LSCRIPT = src/linker.ld
LDHARDFLAGS := $(LDFLAGS)  		      	\
	-static                   			\
	-nostdlib                 			\
	-no-pie                   			\
	-z max-page-size=0x1000   			\
	-T $(LSCRIPT)

QFLAGS = 								\
	-m 4G								\
	-hda $(KERNEL_HDD)					\

QDEBUGFLAGS := $(QFLAGS)				\
	-debugcon file:./log_debug.txt		\
	-D ./log_qemu.txt					\
	-d cpu_reset						\
	-monitor stdio						\

QGDBFLAGS := $(QDEBUGFLAGS)				\
	-S -gdb tcp::9000					\

GDBFLAGS :=								\
	-ix scripts/init_script.gdb			\

.PHONY: disk run clean all debug gdb
.DEFAULT_GOAL = disk

all: run clean

run: disk
	qemu-system-x86_64 $(QFLAGS)
debug: disk
	qemu-system-x86_64 $(QDEBUGFLAGS)
gdb: disk
	qemu-system-x86_64 $(QGDBFLAGS) & konsole -e gdb $(GDBFLAGS)
disk: $(KERNEL_HDD)

$(KERNEL_ELF):
	$(BUILDSCRIPT)

limine/limine-install:
	$(MAKE) -C limine limine-install

$(KERNEL_HDD): limine/limine-install $(KERNEL_ELF)
	mkdir -p build
	rm -f $(KERNEL_HDD)
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD)
	parted -s $(KERNEL_HDD) mklabel msdos
	parted -s $(KERNEL_HDD) mkpart primary 1 100%
	echfs-utils -m -p0 $(KERNEL_HDD) quick-format 32768
	echfs-utils -m -p0 $(KERNEL_HDD) import $(KERNEL_ELF) kernel.elf
	echfs-utils -m -p0 $(KERNEL_HDD) import limine.cfg limine.cfg
	limine/limine-install limine/limine.bin $(KERNEL_HDD)

clean:
	$(CLEANSCRIPT)
