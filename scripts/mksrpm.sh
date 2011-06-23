#!/bin/sh

BUILDNUM=`cat BUILDNUM`
BUILDNUM=`expr $BUILDNUM + 1`
echo $BUILDNUM > BUILDNUM
DATE=`date +"%a %b %d %Y"`
GCC_VERSION=`cat gcc/gcc/BASE-VER`
GDB_VERSION=`cat src/gdb/version.in | sed -e "s/\-//"`
BINUTILS_VERSION=`cat src/bfd/configure.in | awk -F "[,() ]" -f scripts/v.awk` 
NEWLIB_VERSION=`cat src/newlib/configure | awk -F "\"" -f scripts/nv.awk` 
QEMU_VERSION=`cat qemu/config-host.h | awk -F "\"" -f scripts/qv.awk` 

if ! test -d dist; then
  mkdir dist
fi
rm -f dist/*.spec
rm -f dist/*.rpm

for i in elf rtems; do
  cp scripts/moxie-$i-binutils.spec.in dist/moxie-$i-binutils.spec
  sed -i "s/@VERSION@/$BINUTILS_VERSION/g" dist/moxie-$i-binutils.spec
  sed -i "s/@DATE@/$DATE/g" dist/moxie-$i-binutils.spec
  cp scripts/moxie-$i-gcc.spec.in dist/moxie-$i-gcc.spec
  sed -i "s/@VERSION@/$GCC_VERSION/g" dist/moxie-$i-gcc.spec
  sed -i "s/@DATE@/$DATE/g" dist/moxie-$i-gcc.spec
  cp scripts/moxie-$i-newlib.spec.in dist/moxie-$i-newlib.spec
  sed -i "s/@VERSION@/$NEWLIB_VERSION/g" dist/moxie-$i-newlib.spec
  sed -i "s/@DATE@/$DATE/g" dist/moxie-$i-newlib.spec

  sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-$i-binutils.spec
  sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-$i-gcc.spec
  sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-$i-newlib.spec
done;

cp scripts/moxie-rtems.spec.in dist/moxie-rtems.spec
sed -i "s/@VERSION@/4.10.99.0/g" dist/moxie-rtems.spec
sed -i "s/@DATE@/$DATE/g" dist/moxie-rtems.spec
sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-rtems.spec

cp scripts/moxie-elf-qemu.spec.in dist/moxie-elf-qemu.spec
sed -i "s/@VERSION@/$QEMU_VERSION/g" dist/moxie-elf-qemu.spec
sed -i "s/@DATE@/$DATE/g" dist/moxie-elf-qemu.spec
sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-elf-qemu.spec
cp scripts/moxie-elf-gdb.spec.in dist/moxie-elf-gdb.spec
sed -i "s/@VERSION@/$GDB_VERSION/g" dist/moxie-elf-gdb.spec
sed -i "s/@DATE@/$DATE/g" dist/moxie-elf-gdb.spec
sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/moxie-elf-gdb.spec

#cp scripts/kernel.spec.in dist/kernel.spec
#sed -i "s/@DATE@/$DATE/g" dist/kernel.spec

#sed -i "s/@BUILDNUM@/$BUILDNUM/g" dist/kernel.spec

(cd qemu; make clean > /dev/null 2>&1)
tar -czf dist/moxie-elf-qemu.tar.gz qemu

rpmbuild --nodeps --define "VERSION $QEMU_VERSION" --define "_sourcedir dist" --define "_srcrpmdir dist" -bs dist/moxie-elf-qemu.spec


tar \
--exclude CVS \
--exclude src/libgloss \
--exclude src/newlib \
--exclude src/gas \
--exclude src/binutils \
--exclude src/ld \
--exclude src/gold \
--exclude src/gprof \
-czf dist/moxie-gdb.tar.gz src

rpmbuild \
--define "_sourcedir dist" \
--define "_srcrpmdir dist" \
--define "VERSION $BINUTILS_VERSION" \
-bs dist/moxie-elf-gdb.spec

for i in elf rtems; do

(cd gcc; ./contrib/gcc_update --touch)
tar \
--exclude .svn \
--exclude libjava \
--exclude libada \
--exclude gnattools \
--exclude zlib \
--exclude libffi \
--exclude libmudflap \
--exclude fortran \
--exclude boehm-gc \
--exclude libgfortran \
--exclude ada \
--exclude libobjc \
--exclude ppl \
--exclude cloog-ppl \
-czf dist/moxie-gcc.tar.gz gcc

rpmbuild --nodeps --define "VERSION $GCC_VERSION" --define "_sourcedir dist" --define "_srcrpmdir dist" -bs dist/moxie-$i-gcc.spec

tar \
--exclude CVS \
--exclude src/gdb \
--exclude src/sim \
--exclude src/libgloss \
--exclude src/newlib \
--exclude src/readline \
--exclude src/libdecnumber \
--exclude src/gold \
-czf dist/moxie-binutils.tar.gz src

rpmbuild \
--define "_sourcedir dist" \
--define "_srcrpmdir dist" \
--define "VERSION $BINUTILS_VERSION" \
-bs dist/moxie-$i-binutils.spec

tar \
--exclude CVS \
--exclude src/gdb \
--exclude src/sim \
--exclude src/readline \
--exclude src/libdecnumber \
--exclude src/bfd \
--exclude src/binutils \
--exclude src/contrib \
--exclude src/cpu \
--exclude src/CVS \
--exclude src/elfcpp \
--exclude src/etc \
--exclude src/gas \
--exclude src/gdb \
--exclude src/gold \
--exclude src/gprof \
--exclude src/include \
--exclude src/intl \
--exclude src/ld \
--exclude src/libdecnumber \
--exclude src/libiberty \
--exclude src/opcodes \
--exclude src/readline \
--exclude src/sim \
--exclude src/texinfo \
-czf dist/moxie-newlib.tar.gz src

rpmbuild --nodeps \
--define "_sourcedir dist" \
--define "_srcrpmdir dist" \
-bs dist/moxie-$i-newlib.spec

done;

tar \
-czf dist/rtems.tar.gz rtems

rpmbuild --nodeps \
--define "_sourcedir dist" \
--define "_srcrpmdir dist" \
-bs dist/moxie-rtems.spec

#tar \
#-czf dist/linux-2.6.tar.gz linux-2.6
#
#rpmbuild --nodeps \
#--define "_sourcedir dist" \
#--define "_srcrpmdir dist" \
#-bs dist/kernel.spec
