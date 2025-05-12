#!/bin/sh
set -e
. ./iso.sh

#qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso -drive file=my_fat_disk.img,format=raw,media=disk -boot d
