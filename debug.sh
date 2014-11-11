#!/bin/sh

qemu-system-i386 -S -s -m 24M -no-kvm -net none -cdrom build/treeos.iso

