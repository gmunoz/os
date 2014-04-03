#!/bin/bash

# IMAGE_PATH=/home/munozga
# FLOPPY=/media/floppy
# dd if=/dev/zero of=$IMAGE_PATH/floppy.img bs=1024 count=1440
# mke2fs -m 0 -F $IMAGE_PATH/floppy.img
# mount -o loop $IMAGE_PATH/floppy.img $FLOPPY
# mkdir -p $FLOPPY/boot/grub
# cp /usr/lib/grub/i386-pc/stage1 $FLOPPY/boot/grub
# cp /usr/lib/grub/i386-pc/stage2 $FLOPPY/boot/grub
# cp /usr/lib/grub/i386-pc/e2fs_stage1_5 $FLOPPY/boot/grub
# umount $FLOPPY
# echo "(fd2)  $IMAGE_PATH/floppy.img" > /tmp/device.map
# grub --no-curses --device-map=/tmp/device.map
#   > root (fd2)
#   > setup (fd2)
#   > quit
# dd if=$IMAGE_PATH/floppy.img of=/dev/fd0  # Copy to a floppy disk
mount /mnt/os &&
cp kernel.elf /mnt/os &&
umount /mnt/os/
