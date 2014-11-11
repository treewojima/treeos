CC       := /opt/cross-i686/bin/i686-elf-gcc
AR       := /opt/cross-i686/bin/i686-elf-ar

BUILDDIR   := $(PWD)/build
SYSROOT    := $(PWD)/sysroot
INCLUDEDIR := $(SYSROOT)/usr/include
LIBDIR     := $(SYSROOT)/usr/lib
BOOTDIR    := $(SYSROOT)/boot

CFLAGS       := -c -ggdb3 -std=gnu11 -O0 -Wall -Wextra -ffreestanding \
                -Ikernel/include -Ilibck/include --sysroot=$(SYSROOT) \
                -isystem=$(INCLUDEDIR)
ASFLAGS      := $(CFLAGS) -D__treeos_export_asm
LDFLAGS      := -ffreestanding -fbuiltin -nostdlib \
                -T kernel/arch/i386/linker.ld --sysroot=$(SYSROOT) -lck

KERNEL_CSOURCES := $(shell find kernel -type f -name "*.c" -print)
KERNEL_ASOURCES := $(shell find kernel -name "crt?.S" -prune -o -type f -name "*.S" -print)
LIBCK_CSOURCES  := $(shell find libck -type f -name "*.c" -print)
LIBCK_ASOURCES  := $(shell find libck -type f -name "*.S" -print)

CRTI_OBJ     := kernel/arch/i386/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ   := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ     := kernel/arch/i386/crtn.o

KERNEL_OBJECTS := $(patsubst %.c, %.o, $(KERNEL_CSOURCES)) \
                  $(patsubst %.S, %.o, $(KERNEL_ASOURCES))
LIBCK_OBJECTS  := $(patsubst %.c, %.o, $(LIBCK_CSOURCES)) \
                  $(patsubst %.S, %.o, $(LIBCK_ASOURCES))
#DEPFILES := $(patsubst %.c, %.dep, $(CSOURCES))

LIBCK_TARGET  := $(BUILDDIR)/libck.a
KERNEL_TARGET := $(BUILDDIR)/kernel.elf
ISO_TARGET    := $(BUILDDIR)/treeos.iso
GRUB_CFG_FILE := $(BOOTDIR)/grub/grub.cfg

.PHONY: dumpvars all distclean clean clean-kernel clean-libs clean-sysroot \
        install install-headers install-libs install-kernel install-grub iso

all: $(LIBCK_TARGET) $(KERNEL_TARGET)

$(LIBCK_TARGET): $(LIBCK_OBJECTS)
	$(AR) rcs $@ $(LIBCK_OBJECTS)

$(KERNEL_TARGET): $(KERNEL_OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)
	$(CC) -o $(KERNEL_TARGET) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(KERNEL_OBJECTS) $(CRTEND_OBJ) $(CRTN_OBJ) $(LDFLAGS)

distclean: clean clean-sysroot
	rm -f $(ISO_TARGET)

clean: clean-kernel clean-libs

clean-kernel:
	rm -f $(KERNEL_TARGET) $(KERNEL_OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)
	@find kernel -type f -iname "*~" -exec rm -f {} \;

clean-libs:
	rm -f $(LIBCK_TARGET) $(LIBCK_OBJECTS)
	@find libck -type f -iname "*~" -exec rm -f {} \;

clean-sysroot:
	rm -rf $(SYSROOT)

#i-include $(DEPFILES)

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $<

%.o: %.S
	$(CC) -o $@ $(ASFLAGS) $<

install: install-headers install-libs install-kernel

install-headers:
	mkdir -p $(INCLUDEDIR)
	cp -RTv kernel/include $(INCLUDEDIR)
	cp -RTv libck/include $(INCLUDEDIR)

install-libs: $(LIBCK_TARGET)
	mkdir -p $(LIBDIR)
	cp $(LIBCK_TARGET) $(LIBDIR)

install-kernel: $(KERNEL_TARGET)
	mkdir -p $(BOOTDIR)
	cp $(KERNEL_TARGET) $(BOOTDIR)

install-grub:
	mkdir -p $(BOOTDIR)/grub
	echo "set default = 0" > $(GRUB_CFG_FILE)
	echo "set timeout = 2\n" >> $(GRUB_CFG_FILE)
	echo "menuentry \"treeos\" {" >> $(GRUB_CFG_FILE)
	echo "    multiboot /boot/kernel.elf" >> $(GRUB_CFG_FILE)
	echo "}\n" >> $(GRUB_CFG_FILE)

$(ISO_TARGET): install install-grub
	grub-mkrescue -o $(ISO_TARGET) $(SYSROOT)

iso: $(ISO_TARGET)

