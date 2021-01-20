DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

KERNEL=jamesos.kernel

DESTDIR?=
PREFIX?=/usr/local
INCLUDEDIR?=$(PREFIX)/include

NASMFLAGS:=-felf64
CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -I../stivale -I. -fno-stack-protector    \
				  -fno-pic -fomit-frame-pointer -mno-80387 -mno-mmx -mno-3dnow -mno-sse \
				  -mno-sse2 -masm=intel -mcmodel=kernel
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS) -static -nostdlib -no-pie -z max-page-size=0x1000
LIBS:=$(LIBS) -nostdlib -lk -lgcc

KERNEL_OBJS=\
kernel/kernel.o \
kernel/kinfo.o \
kernel/kdebug.o \
devices/terminal.o \
kernel/interrupts.o \
kernel/irqhandler.o \
kernel/idt.o \
kernel/gdt.o \
kernel/load_gdt.o

OBJS=\
$(KERNEL_OBJS)
# The crt objects were here

LINK_LIST=\
$(LDFLAGS) \
$(KERNEL_OBJS) \
$(LIBS)
# The objects were also here

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .asm

all: $(KERNEL)

$(KERNEL): $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	#grub-file --is-x86-multiboot myos.kernel

#$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
#	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.asm.o:
	nasm $< -o $@ $(NASMFLAGS)

#.S.o:
#	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(KERNEL)
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers $(KERNEL)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

-include $(OBJS:.o=.d)
