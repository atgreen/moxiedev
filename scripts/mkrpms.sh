#!/bin/sh

# For this to work, the mock config file for $TARGET must point to the 
# local yum repo at $REPO.

TARGET=epel-4-i386
RESULTDIR=/var/lib/mock/$TARGET/result
REPO=dist/MoxieLogic

mkdir -p $REPO/RPMS/i386/debuginfo
mkdir -p $REPO/RPMS/noarch
mkdir -p $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-binutils-*src.rpm
mv $RESULTDIR/moxie-elf-binutils-debuginfo*i386.rpm $REPO/RPMS/i386/debuginfo
mv $RESULTDIR/moxie-elf-binutils-*i386.rpm $REPO/RPMS/i386
mv $RESULTDIR/moxie-elf-binutils-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-gcc-*src.rpm
mv $RESULTDIR/moxie-elf-gcc-debuginfo*i386.rpm $REPO/RPMS/i386/debuginfo
mv $RESULTDIR/moxie-elf-gcc-*i386.rpm $REPO/RPMS/i386
mv $RESULTDIR/moxie-elf-gcc-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-newlib-*src.rpm
mv $RESULTDIR/moxie-elf-newlib-*noarch.rpm $REPO/RPMS/noarch
mv $RESULTDIR/moxie-elf-newlib-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-gdb-*src.rpm
mv $RESULTDIR/moxie-elf-gdb-debuginfo*i386.rpm $REPO/RPMS/i386/debuginfo
mv $RESULTDIR/moxie-elf-gdb-*i386.rpm $REPO/RPMS/i386
mv $RESULTDIR/moxie-elf-gdb-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-qemu-*src.rpm
mv $RESULTDIR/moxie-elf-qemu-debuginfo*i386.rpm $REPO/RPMS/i386/debuginfo
mv $RESULTDIR/moxie-elf-qemu-*i386.rpm $REPO/RPMS/i386
mv $RESULTDIR/moxie-elf-qemu-*src.rpm  $REPO/SRPMS
createrepo $REPO
