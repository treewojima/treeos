HOST     ?= $(shell ../default-host.sh)
HOSTARCH := $(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS   ?= -O0 -ggdb3
CPPFLAGS ?=
LDFLAGS  ?=
LIBS     ?=

DESTDIR     ?=
PREFIX      ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR     ?= $(EXEC_PREFIX)/boot
INCLUDEDIR  ?= $(PREFIX)/include

CFLAGS   := $(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra
CPPFLAGS := $(CPPFLAGS) -D__is_treeos_kernel -Iinclude
LDFLAGS  := $(LDFLAGS)
LIBS     := $(LIBS) -nostdlib -lk -lgcc

ARCHDIR := arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS   := $(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS  := $(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS     := $(LIBS) $(KERNEL_ARCH_LIBS)

CRTI_OBJ     := $(ARCHDIR)/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ   := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ     := $(ARCHDIR)/crtn.o

KERNEL_OBJS := $(KERNEL_ARCH_OBJS) \
               kernel/heap.o       \
               kernel/kernel.o     \
               kernel/panic.o      \
               kernel/reboot.o

ORDERED_OBJS := $(CRTI_OBJ)     \
	        $(CRTBEGIN_OBJ) \
                $(KERNEL_OBJS)  \
	        $(CRTEND_OBJ)   \
	        $(CRTN_OBJ)

KERNEL := kernel.elf

all: $(KERNEL)

.PHONY: all clean install install-headers install-kernel

$(KERNEL): $(ORDERED_OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(ORDERED_OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
        $(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS) $(CRTI_OBJ) $(CRTN_OBJ)

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)

install-kernel: $(KERNEL)
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp $(KERNEL) $(DESTDIR)$(BOOTDIR)

