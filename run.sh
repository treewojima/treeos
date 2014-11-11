#!/bin/sh

#cd /home/scott/documents/programming/os
qemu-system-i386 -m 24M -no-kvm -net none -cdrom build/treeos.iso

