CC       := /opt/cross-i686/bin/i686-elf-gcc
AR       := /opt/cross-i686/bin/i686-elf-ar
LD       := /opt/cross-i686/bin/i686-elf-ld

SYSROOT    := $(PWD)/sysroot
INCLUDEDIR := $(SYSROOT)/usr/include
LIBDIR     := $(SYSROOT)/usr/lib
BOOTDIR    := $(SYSROOT)/boot

WARNINGS := -Wall -Wextra -Wpedantic -Werror -Wshadow
CFLAGS   := -c -pipe -ggdb3 -std=gnu11 -O0 $(WARNINGS) -fdiagnostics-color=always \
            -ffreestanding -Ikernel/include -Ilibc/include -Ilibk/include \
            --sysroot=$(SYSROOT) -DTREEOS_I386 \
            -DNOT_IN_QT_CREATOR
ASFLAGS  := $(CFLAGS) -DTREEOS_EXPORT_ASM
LDFLAGS  := -ffreestanding -fno-common -nostdlib \
           -T kernel/arch/i386/linker.ld --sysroot=$(SYSROOT) -lc -lk

KERNEL_CSOURCES := $(shell find kernel -type f -name "*.c" -print)
KERNEL_ASOURCES := $(shell find kernel -iname "crt?.S" -prune -o -type f -iname "*.S" -print)
LIBC_CSOURCES   := $(shell find libc -type f -name "*.c" -print)
LIBC_ASOURCES   := $(shell find libc -type f -iname "*.S" -print)
LIBK_CSOURCES   := $(shell find libk -type f -name "*.c" -print)
LIBK_ASOURCES   := $(shell find libk -type f -iname "*.S" -print)

CRTI_OBJ     := kernel/arch/i386/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ   := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ     := kernel/arch/i386/crtn.o

KERNEL_OBJECTS := $(patsubst %.c, %.o, $(KERNEL_CSOURCES)) \
                  $(patsubst %.S, %.o, $(KERNEL_ASOURCES))
LIBC_OBJECTS   := $(patsubst %.c, %.o, $(LIBC_CSOURCES)) \
                  $(patsubst %.S, %.o, $(LIBC_ASOURCES))
LIBK_OBJECTS   := $(patsubst %.c, %.o, $(LIBK_CSOURCES)) \
                  $(patsubst %.S, %.o, $(LIBK_ASOURCES))
#DEPFILES := $(patsubst %.c, %.dep, $(CSOURCES))

LIBC_TARGET   := $(LIBDIR)/libc.a
LIBK_TARGET   := $(LIBDIR)/libk.a
KERNEL_TARGET := $(BOOTDIR)/kernel.elf
ISO_TARGET    := $(PWD)/treeos.iso
GRUB_CFG_FILE := $(BOOTDIR)/grub/grub.cfg

.PHONY: dumpvars all kernel libs libc libk distclean clean clean-kernel \
        clean-libs clean-iso install install-headers install-libs \
		install-kernel install-grub iso

all: libs kernel

libs: libc libk

libc: $(LIBC_TARGET)

libk: $(LIBK_TARGET)

kernel: libc libk $(KERNEL_TARGET)

$(LIBC_TARGET): $(LIBC_OBJECTS)
	mkdir -p $(LIBDIR)
	$(AR) rcs $@ $(LIBC_OBJECTS)

$(LIBK_TARGET): $(LIBK_OBJECTS)
	mkdir -p $(LIBDIR)
	$(AR) rcs $@ $(LIBK_OBJECTS)

$(KERNEL_TARGET): $(KERNEL_OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)
	mkdir -p $(BOOTDIR)
	$(CC) -o $(KERNEL_TARGET) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(KERNEL_OBJECTS) $(CRTEND_OBJ) $(CRTN_OBJ) $(LDFLAGS)

distclean: clean clean-iso

clean: clean-kernel clean-libs

clean-kernel:
	rm -f $(KERNEL_TARGET) $(KERNEL_OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)
	@find kernel -type f -iname "*~" -exec rm -f {} \;

clean-libs:
	rm -f $(LIBC_TARGET) $(LIBK_TARGET) $(LIBC_OBJECTS) $(LIBK_OBJECTS)
	@find libc -type f -iname "*~" -exec rm -f {} \;

clean-iso:
	rm -f $(ISO_TARGET)

#i-include $(DEPFILES)

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $<

%.o: %.S
	$(CC) -o $@ $(ASFLAGS) $<

install-grub:
	mkdir -p $(BOOTDIR)/grub
	echo "set default=0" > $(GRUB_CFG_FILE)
	echo "set timeout=1" >> $(GRUB_CFG_FILE)
	echo "menuentry \"treeos\" {" >> $(GRUB_CFG_FILE)
	echo "    multiboot /boot/kernel.elf" >> $(GRUB_CFG_FILE)
	echo "}" >> $(GRUB_CFG_FILE)

$(ISO_TARGET): kernel install-grub
	grub2-mkrescue -o $(ISO_TARGET) $(SYSROOT)

iso: $(ISO_TARGET)

