#!/bin/sh

qemu-system-i386 -S -s -m 32M -no-kvm -net none -cdrom build/treeos.iso -display sdl

