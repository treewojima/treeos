#!/bin/sh

#cd /home/scott/documents/programming/os
qemu-system-i386 -m 128M -no-kvm -net none -cdrom build/treeos.iso -display sdl

