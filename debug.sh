#!/bin/sh

qemu-system-i386 -S -s -m 32M -no-kvm -net none -cdrom treeos.iso -display sdl &!
i686-elf-gdb

