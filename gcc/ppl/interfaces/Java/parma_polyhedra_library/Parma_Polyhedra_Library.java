/* Parma_Polyhedra_Library Java class declaration and implementation.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

/*!
  \defgroup PPL_Java_interface Java Language Interface

  The Parma Polyhedra Library comes equipped with an interface for the
  Java language.
*/

/*! \mainpage
  The Parma Polyhedra Library comes equipped with an interface for the
  Java language.
  The Java interface provides access to the numerical abstractions
  (convex polyhedra, BD shapes, octagonal shapes, etc.) implemented
  by the PPL library.
  A general introduction to the numerical abstractions,
  their representation in the PPL and the operations provided
  by the PPL is given in the main \extref{preamble, PPL user manual}.
  Here we just describe those aspects that are specific to the Java interface.
  In the sequel, \c prefix is the path prefix under which
  the library has been installed (typically \c /usr or \c /usr/local).

  <H2>Overview</H2>

  Here is a list of notes with general information and advice
  on the use of the Java interface.

  - The numerical abstract domains available to the Java user as
    Java classes consist of the <EM>simple</EM> domains,
    <EM>powersets</EM> of a simple domain and <EM>products</EM>
    of simple domains. Note that the default configuration will
    only enable a subset of these domains (if you need a different
    set of domains, see configuration option
    <code>--enable-instantiations</code>).
    - The simple domains are:
      - convex polyhedra, which consist of C_Polyhedron and
        NNC_Polyhedron;<BR>
      - weakly relational, which consist of BD_Shape_N and
        Octagonal_Shape_N
        where N is one of the numeric types
        signed_char, short, int, long, long_long,
        mpz_class, mpq_class;<BR>
      - boxes which consist of
        Int8_Box, Int16_Box, Int32_Box, Int64_Box,
        Uint8_Box, Uint16_Box, Uint32_Box, Uint64_Box,
        Float_Box, Double_Box, Long_Double_Box,
        Z_Box, Rational_Box; and<BR>
      - the Grid domain.
    - The powerset domains are Pointset_Powerset_S where S is
      a simple domain.
    - The product domains consist of
      Direct_Product_S_T,
      Smash_Product_S_T and
      Constraints_Product_S_T where S
      and T are simple domains.

  - In the following, any of the above numerical
    abstract domains  is called a PPL <EM>domain</EM>
    and any element of a PPL domain is called a <EM>PPL object</EM>.
  - The Java interface files are all installed in the directory
    \c prefix/lib/ppl.  Since this includes shared and
    dynamically loaded libraries, you must make your dynamic
    linker/loader aware of this fact.  If you use a GNU/Linux system,
    try the commands <CODE>man ld.so</CODE> and <CODE>man ldconfig</CODE>
    for more information.
  - A Java program can create a new object for a PPL domain by
    using the constructors for the class corresponding to the domain.
  - For a PPL object with space dimension \p k,
    the identifiers used for the PPL variables
    must lie between 0 and \f$k-1\f$ and correspond to the indices of the
    associated Cartesian axes.
    For example, when using methods that combine PPL polyhedra
    or add constraints or generators to a representation of
    a PPL polyhedron,
    the polyhedra referenced and any constraints or generators in the call
    should follow all the (space) dimension-compatibility rules stated in
    Section \extref{representation, Representations of Convex Polyhedra}
    of the main PPL user manual.
  - As explained above, a polyhedron has a fixed topology C or NNC,
    that is determined at the time of its initialization.
    All subsequent operations on the polyhedron must respect all the
    topological compatibility rules stated in Section
    \extref{representation, Representations of Convex Polyhedra}
    of the main PPL user manual.
  - Any application using the PPL should make sure that only the
    intended version(s) of the library are ever used.
  - When the Parma Polyhedra Library is configured, it will automatically
    test for the existence of the Java system (unless configuration options
    are passed to disable the build of the Java interface;
    see configuration option <code>--enable-interfaces</code>).
    If Java is correctly installed in a standard location, things will be
    arranged so that the Java interface is built and installed
    (see configuration option <code>--with-java</code> if you need to
    specify a non-standard location for the Java system).

*/ /* \mainpage */

/*! \brief The PPL Java interface package.

  \ingroup PPL_Java_interface
  All classes, interfaces and enums related to the Parma Polyhedra Library
  Java interface are included in this package.
*/
package parma_polyhedra_library;

//! A class collecting library-level functions.
/*! \ingroup PPL_Java_interface */
public class Parma_Polyhedra_Library {

    //! \name Version Checking
    //@{

    //! Returns the major number of the PPL version.
    public static native int version_major();

    //! Returns the minor number of the PPL version.
    public static native int version_minor();

    //! Returns the revision number of the PPL version.
    public static native int version_revision();

    //! Returns the beta number of the PPL version.
    public static native int version_beta();

    //! Returns a string containing the PPL version.
    public static native String version();

    //! Returns a string containing the PPL banner.
    /*!
      The banner provides information about the PPL version, the licensing,
      the lack of any warranty whatsoever, the C++ compiler used to build
      the library, where to report bugs and where to look for further
      information.
    */
    public static native String banner();

    //@} // Version Checking

    //! \name (Re-) Setting floating-point rounding mode.
    //@{

    /*! \brief
      Sets the FPU rounding mode so that the PPL abstractions based on
      floating point numbers work correctly.

      This is performed automatically at initialization-time.  Calling
      this function is needed only if restore_pre_PPL_rounding() has been
      previously called.
    */
    public static native void set_rounding_for_PPL();

    /*! \brief
      Sets the FPU rounding mode as it was before initialization of the PPL.

      After calling this function it is absolutely necessary to call
      set_rounding_for_PPL() before using any PPL abstractions based on
      floating point numbers.
      This is performed automatically at finalization-time.
    */
    public static native void restore_pre_PPL_rounding();

    //@} // (Re-) Setting floating-point rounding mode.

}
