#!/bin/sh
set -x
# For this to work, the mock config file for $TARGET must point to the 
# local yum repo at $REPO.

#CPU=x86_64
CPU=i386

TAG=el5
TARGET=epel-5-$CPU
RESULTDIR=/var/lib/mock/$TARGET/result
REPO=dist/MoxieLogic/$TAG

mkdir -p $REPO/RPMS/$CPU/debuginfo
mkdir -p $REPO/RPMS/noarch
mkdir -p $REPO/SRPMS
createrepo $REPO

for i in moxie-elf-binutils moxie-elf-gcc moxie-elf-newlib moxie-elf-gdb moxie-qemu moxie-rtems-binutils moxie-rtems-newlib moxie-rtems-gcc moxie-rtems; do
  mock -r $TARGET dist/$i-[0-9]*src.rpm;
  FILE=`ls $RESULTDIR/$i-*.rpm | head -1`
  if test x$FILE != "x"; then
    rm $REPO/RPMS/noarch/$i-[0-9]*.rpm
    mv $RESULTDIR/$i-*src.rpm  $REPO/SRPMS;
    case `ls $RESULTDIR/$i-*.rpm` in
    *noarch*)
      rm $REPO/RPMS/noarch/$i-[0-9]*.rpm
      mv $RESULTDIR/$i-*.rpm $REPO/RPMS/noarch;
      ;;
    *)
      rm $REPO/RPMS/$CPU/debuginfo/$i-debuginfo*.rpm
      rm $REPO/RPMS/$CPU/$i-[0-9]*.rpm
      mv $RESULTDIR/$i-debuginfo*.rpm $REPO/RPMS/$CPU/debuginfo;
      mv $RESULTDIR/$i-*.rpm $REPO/RPMS/$CPU;
      ;;
    esac
    createrepo $REPO;
  fi;
done

