#!/bin/sh

# 1010101010101010101010101010101010101010101010101010101010101010101010101010101
# 0101010101010101010101010101010101010101010101010101010101010101010101010101010
# 1   ___  ___          _     ______                                            1
# 0   |  \/  |         (_)    |  _  \              Open Source Tools,           0
# 1   | .  . | _____  ___  ___| | | |_____   __    Firmware, and HDL code for   1
# 0   | |\/| |/ _ \ \/ / |/ _ \ | | / _ \ \ / /    the Moxie processor core.    0
# 1   | |  | | (_) >  <| |  __/ |/ /  __/\ V /                                  1
# 0   \_|  |_/\___/_/\_\_|\___|___/ \___| \_/      http://moxielogic.org/blog   0
# 1                                                                             1
# 0101010101010101010101010101010101010101010101010101010101010101010101010101010
# 1010101010101010101010101010101010101010101010101010101010101010101010101010101

# This script is intended to be used as the daily build cron job on
# the FSF's GCC Compiler Farm.  It builds the latest GCC, binutils,
# simulator, and device tree compiler from upstream sources, runs the
# GCC regression tester (make check), and emails the results to the
# gcc-testresults mailing list.

# See the following URLs for more info:
#   http://gcc.gnu.org/wiki/CompileFarm 
#   http://gcc.gnu.org/ml/gcc-testresults


PREFIX=$HOME/root/usr

rm -rf $HOME/root
rm -rf $HOME/*moxie-elf
rm -rf $HOME/dtc
mkdir -p $PREFIX/share/moxie

cat > $PREFIX/share/moxie/site.exp <<EOF
# Make sure we look in the right place for the board description files.
if ![info exists boards_dir] {
    set boards_dir {}
}

lappend boards_dir [pwd]/../../../../../root/usr/share/moxie
lappend boards_dir [pwd]/../../../../root/usr/share/moxie

verbose "Global Config File: target_triplet is $target_triplet" 2
global target_list

case "$target_triplet" in {
    { "moxie-elf" } {
	set target_list "moxie-sim"
    }
}
EOF

cat > $PREFIX/share/moxie/moxie-sim.exp <<EOF
# This is a list of toolchains that are supported on this board.
set_board_info target_install {moxie-elf}

# Load the generic configuration for this board. This will define a basic set
# of routines needed by the tool to communicate with the board.
load_generic_config "sim"

# basic-sim.exp is a basic description for the standard Cygnus simulator.
load_base_board_description "basic-sim"

# "moxie" is the name of the sim subdir in devo/sim.
setup_sim moxie

# No multilib options needed by default.
process_multilib_options ""

# We only support newlib on this target. We assume that all multilib
# options have been specified before we get here.

set_board_info compiler  "[find_gcc]"
set_board_info cflags    "[libgloss_include_flags] [newlib_include_flags]"
set_board_info ldflags   "[libgloss_link_flags] [newlib_link_flags]"
# No linker script needed.
set_board_info ldscript "-Tsim.ld"

# Configuration settings for testsuites
set_board_info noargs 1
set_board_info gdb,nosignals 1
set_board_info gdb,noresults 1
set_board_info gdb,cannot_call_functions 1
set_board_info gdb,skip_float_tests 1
set_board_info gdb,can_reverse 1
set_board_info gdb,use_precord 1

# More time is needed
set_board_info gcc,timeout 800
set_board_info gdb,timeout 60

# Used by a few gcc.c-torture testcases to delimit how large the stack can
# be.
set_board_info gcc,stack_size  5000
EOF

# Check out the binutils and newlib sources.
cvs -z9 -d:pserver:anoncvs@sourceware.org:/cvs/src co binutils newlib sim
# Check out the gcc sources.
svn co svn://gcc.gnu.org/svn/gcc/trunk gcc
# Check out the device tree compiler
git clone http://jdl.com/software/dtc.git

# Build the device tree compiler.  This is required in order to build
# the simulator.
(cd dtc;
 make; 
 mkdir -p $PREFIX/bin; 
 cp dtc $PREFIX/bin)

export PATH=$PREFIX/bin:$PATH

# Build moxie-elf binutils and the simulator.
mkdir binutils-moxie-elf
(cd binutils-moxie-elf;
 ../src/configure --prefix=$PREFIX --target=moxie-elf;
 make all-binutils all-gas all-ld;
 make install-binutils install-gas install-ld;
 make all-sim;
 make install-sim;)

 Build initial gcc for newlib build.
mkdir gcc-for-newlib-moxie-elf
(cd gcc-for-newlib-moxie-elf;
 ../gcc/configure --prefix=$PREFIX --target=moxie-elf \
   --enable-languages=c \
   --disable-libssp \
   --with-newlib \
   --enable-lto \
   --with-libelf=/opt/cfarm/libelf-0.8.12 \
   --with-mpfr=/opt/cfarm/mpfr-2.4.1 \
   --with-gmp=/opt/cfarm/gmp-4.2.4 \
   --with-mpc=/opt/cfarm/mpc-0.8;
 make;
 make install)

mkdir newlib-moxie-elf
(cd newlib-moxie-elf;
 ../src/configure --prefix=$PREFIX --target=moxie-elf;
 make all-target-newlib all-target-libgloss CC_FOR_TARGET=moxie-elf-gcc;
 make install-target-newlib install-target-libgloss CC_FOR_TARGET=moxie-elf-gcc)

# Build full gcc
mkdir gcc-moxie-elf
(cd gcc-moxie-elf;
 ../gcc/configure --prefix=$PREFIX --target=moxie-elf \
   --enable-languages=c,c++ \
   --disable-libssp \
   --with-newlib \
   --with-headers=$PREFIX/moxie-elf/include \
   --enable-lto \
   --with-libelf=/opt/cfarm/libelf-0.8.12 \
   --with-mpfr=/opt/cfarm/mpfr-2.4.1 \
   --with-gmp=/opt/cfarm/gmp-4.2.4 \
   --with-mpc=/opt/cfarm/mpc-0.8;
 make;
 make install)

# Test GCC and mail results.
(cd gcc-moxie-elf;
 PATH=$PREFIX/bin:$PATH DEJAGNU=$PREFIX/share/moxie/site.exp make check "RUNTESTFLAGS=--target_board moxie-sim";
 ../gcc/contrib/test_summary | sh;)
