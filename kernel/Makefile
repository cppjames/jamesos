DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST

CFLAGS?=-O2 -g -std=gnu11 
CPPFLAGS?=
LDFLAGS?=
LIBS?=

KERNEL=jamesos.kernel

DESTDIR?=
PREFIX?=/usr/local
INCLUDEDIR?=$(PREFIX)/include

ALL_HEADER_DIRS := $(wildcard include/*)
HEADER_DIRS     := $(filter-out include/kernel, $(ALL_HEADER_DIRS))

INCLUDE_PATHS := ../stivale     \
                 .              \

INCLUDE_FLAGS := $(INCLUDE_PATHS:%=-I%)

NASMFLAGS:=-felf64 -g
CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra $(INCLUDE_FLAGS) -fno-stack-protector    \
				  -fno-pic -fomit-frame-pointer -mno-80387 -mno-mmx -mno-3dnow -mno-sse \
				  -mno-sse2 -masm=intel -mcmodel=kernel
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS) -static -nostdlib -no-pie -z max-page-size=0x1000
LIBS:=$(LIBS) -nostdlib -lk -lgcc


CSRC := $(shell find . -name '*.c')
ASMSRC := $(shell find . -name '*.asm')
KERNEL_OBJS := $(CSRC:.c=_c.o) \
               $(ASMSRC:.asm=_asm.o)

OBJS=\
$(KERNEL_OBJS)

LINK_LIST=\
$(LDFLAGS) \
$(KERNEL_OBJS) \
$(LIBS)

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: _c.o _asm.o .c .S .asm

all: $(KERNEL)

$(KERNEL): $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

.c_c.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.asm_asm.o:
	nasm $< -o $@ $(NASMFLAGS)

clean:
	rm -f $(KERNEL)
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers $(KERNEL)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps $(HEADER_DIRS) $(DESTDIR)$(INCLUDEDIR)/.

-include $(OBJS:.o=.d)
