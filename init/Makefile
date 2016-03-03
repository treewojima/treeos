CC       := /opt/cross-i686/bin/i686-elf-gcc
AR       := /opt/cross-i686/bin/i686-elf-ar
LD       := /opt/cross-i686/bin/i686-elf-ld

WARNINGS := -Wall -Wextra -Wpedantic -Werror -Wshadow -Wno-error=unused-function
CFLAGS   := -c -pipe -std=gnu11 -O0 $(WARNINGS) -fdiagnostics-color=always \
            -ffreestanding 
ASFLAGS  := $(CFLAGS)
LDFLAGS  := -ffreestanding -fno-common -nostdlib \
            -T linker.ld

CSOURCES     := $(shell find . -type f -name "*.c" -print)
ASOURCES     := $(shell find . -type f -name "crt?.S" -prune -o -type f -name "*.S" -print)
CRTI_OBJ     := crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ   := $(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ     := crtn.o

OBJECTS := $(patsubst %.c, %.o, $(CSOURCES)) $(patsubst %.S, %.o, $(ASOURCES))

TARGET := init

.PHONY: all install clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)
	@echo ">> Linking executable $(TARGET)"
	$(CC) -o $(TARGET) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJECTS) $(CRTEND_OBJ) $(CRTN_OBJ) $(LDFLAGS)

install:
	cp $(TARGET) ../sysroot/boot/

clean:
	rm -f $(TARGET) $(OBJECTS) $(CRTI_OBJ) $(CRTN_OBJ)

%.o: %.c
	@echo "(CC) $<"
	@$(CC) -o $@ $(CFLAGS) $<

%.o: %.S
	@echo "(AS) $<"
	@$(CC) -o $@ $(ASFLAGS) $<

