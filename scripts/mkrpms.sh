#!/bin/sh

# For this to work, the mock config file for $TARGET must point to the 
# local yum repo at $REPO.

#CPU=x86_64
CPU=i386

TARGET=fedora-12-$CPU
RESULTDIR=/var/lib/mock/$TARGET/result
REPO=dist/MoxieLogic/$CPU

mkdir -p $REPO/RPMS/$CPU/debuginfo
mkdir -p $REPO/RPMS/noarch
mkdir -p $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-binutils-*src.rpm
mv $RESULTDIR/moxie-elf-binutils-debuginfo*.rpm $REPO/RPMS/$CPU/debuginfo
mv $RESULTDIR/moxie-elf-binutils-*.rpm $REPO/RPMS/$CPU
mv $RESULTDIR/moxie-elf-binutils-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-gcc-*src.rpm
mv $RESULTDIR/moxie-elf-gcc-debuginfo*.rpm $REPO/RPMS/$CPU/debuginfo
mv $RESULTDIR/moxie-elf-gcc-*.rpm $REPO/RPMS/$CPU
mv $RESULTDIR/moxie-elf-gcc-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-newlib-*src.rpm
mv $RESULTDIR/moxie-elf-newlib-*noarch.rpm $REPO/RPMS/noarch
mv $RESULTDIR/moxie-elf-newlib-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-gdb-*src.rpm
mv $RESULTDIR/moxie-elf-gdb-debuginfo*.rpm $REPO/RPMS/$CPU/debuginfo
mv $RESULTDIR/moxie-elf-gdb-*.rpm $REPO/RPMS/$CPU
mv $RESULTDIR/moxie-elf-gdb-*src.rpm  $REPO/SRPMS
createrepo $REPO

mock -r $TARGET dist/moxie-elf-qemu-*src.rpm
mv $RESULTDIR/moxie-elf-qemu-debuginfo*.rpm $REPO/RPMS/$CPU/debuginfo
mv $RESULTDIR/moxie-elf-qemu-*.rpm $REPO/RPMS/$CPU
mv $RESULTDIR/moxie-elf-qemu-*src.rpm  $REPO/SRPMS
createrepo $REPO
