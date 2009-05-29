# Automake source file for the Parma Polyhedra Library.
# Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>
#
# This file is part of the Parma Polyhedra Library (PPL).
#
# The PPL is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# The PPL is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
# USA.
#
# For the most up-to-date information see the Parma Polyhedra Library
# site: http://www.cs.unipr.it/ppl/ .

if VALGRIND_TESTS_ENABLED

CHECKER = \
$(VALGRIND) --tool=memcheck -q --trace-children=yes --leak-check=yes \
--num-callers=100 --leak-resolution=high \
--suppressions=$(top_srcdir)/tests/valgrind_suppressions

else !VALGRIND_TESTS_ENABLED

CHECKER =

endif !VALGRIND_TESTS_ENABLED

TESTS_ENVIRONMENT = $(CHECKER)

AM_CPPFLAGS = \
-I$(top_builddir)/src \
-I$(top_srcdir)/src -I$(top_srcdir)/tests \
-I$(top_srcdir)/utils \
@extra_includes@ \
@debug_flag@

LDADD = \
$(top_builddir)/utils/libppl_utils.a \
$(top_builddir)/tests/libppl_tests.a \
$(top_builddir)/src/libppl.la \
@extra_libraries@

TESTS = \
asciidumpload1 \
directproduct1 directproduct2 directproduct3 directproduct4 \
directproduct5 directproduct6 \
partiallyreducedproduct1 partiallyreducedproduct2 partiallyreducedproduct3 \
partiallyreducedproduct4

XFAIL_TESTS =

#
# Sources for the tests
#

asciidumpload1_SOURCES = asciidumpload1.cc

directproduct1_SOURCES = directproduct1.cc
directproduct2_SOURCES = directproduct2.cc
directproduct3_SOURCES = directproduct3.cc
directproduct4_SOURCES = directproduct4.cc
directproduct5_SOURCES = directproduct5.cc
directproduct6_SOURCES = directproduct6.cc

partiallyreducedproduct1_SOURCES = partiallyreducedproduct1.cc
partiallyreducedproduct2_SOURCES = partiallyreducedproduct2.cc
partiallyreducedproduct3_SOURCES = partiallyreducedproduct3.cc
partiallyreducedproduct4_SOURCES = partiallyreducedproduct4.cc

check_PROGRAMS = $(TESTS)

MOSTLYCLEANFILES = \
ascii_dump_load1.dat

$(top_builddir)/utils/libppl_utils.a:
	$(MAKE) $(AM_MAKEFLAGS) -C $(top_builddir)/utils libppl_utils.a

$(top_builddir)/tests/libppl_tests.a:
	$(MAKE) $(AM_MAKEFLAGS) -C $(top_builddir)/tests libppl_tests.a

$(top_builddir)/src/libppl.la:
	$(MAKE) $(AM_MAKEFLAGS) -C $(top_builddir)/src libppl.la
