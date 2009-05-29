/* Generator Java class declaration and implementation.
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

package parma_polyhedra_library;

import java.io.Writer;
import java.io.IOException;

import java.util.*;
import javax.management.RuntimeErrorException;

//! A line, ray, point or closure point.
/*! \ingroup PPL_Java_interface
  An object of the class Generator is one of the following:
  - a line;
  - a ray;
  - a point;
  - a closure point.
*/
public class Generator {
    //! The denominator used if the generator is a point or a clousure point.
    private Coefficient den;

    //! The generator type.
    private Generator_Type gt;

    //! The linear expression.
    private Linear_Expression le;

    /*! \brief
      Builds a generator of type \p g_type, stealing the coefficients from
      \p e.
    */
    private Generator(Linear_Expression e, Generator_Type g_type) {
	le = e.clone();
        gt = g_type;
    }

    //! Returns the closure point at \p e / \p d.
    /*!
      \exception RuntimeErrorException
      Thrown if \p d is zero.
    */
    public static Generator closure_point(Linear_Expression e,
					   Coefficient c) {
        if (c.getBigInteger().equals(java.math.BigInteger.ZERO)) {
            Error cause = new Error("parma_polyhedra_library.Generator::"
                                    + "Generator(le, c):\n"
				    + "the divisor can not be zero.");
            throw new RuntimeErrorException(cause);
        }

        Generator g = new Generator(e, Generator_Type.CLOSURE_POINT);
        g.den = c;
        return g;
    }

    //! Returns the line of direction \p e.
    /*!
      \exception RuntimeErrorException
      Thrown if the homogeneous part of \p e represents the origin of
      the vector space.
    */
    public static Generator line(Linear_Expression le) {
        return new Generator(le, Generator_Type.LINE);
    }

    //! Returns the point at \p e / \p d.
    /*!
      \exception RuntimeErrorException
      Thrown if \p d is zero.
    */
    public static Generator point(Linear_Expression le, Coefficient d) {
        if (d.getBigInteger().equals(java.math.BigInteger.ZERO)) {
	    Error cause = new Error("parma_polyhedra_library.Generator::"
                                    + "Generator(le, d):\n"
				    + "the divisor can not be zero.");
            throw new RuntimeErrorException(cause);
        }

        Generator g = new Generator(le, Generator_Type.POINT);
        g.den = d;
        return g;
    }

    //! Returns the ray of direction \p e.
    /*!
      \exception RuntimeErrorException
      Thrown if the homogeneous part of \p e represents the origin of
      the vector space.
    */
    public static Generator ray(Linear_Expression le) {
        return new Generator(le, Generator_Type.RAY);
    }

    //! If \p this is either a point or a closure point, returns its divisor.
    /*!
      \exception RuntimeErrorException
      Thrown if \p this is neither a point nor a closure point.
    */
    public Coefficient divisor() {
        if (this.gt == Generator_Type.POINT
	    || this.gt == Generator_Type.CLOSURE_POINT)
	    return den;
	Error cause = new Error("parma_polyhedra_library.Generator::divisor:\n"
				+ "this is neither a point"
				+ " nor a closure point.");
	throw new RuntimeErrorException(cause);
    }

    //! Allows to copy the fields from \p g to \p this.
    private void set(Generator g) {
        this.le = g.le;
        this.gt = g.gt;
    }

    //! Returns an ascii formatted internal representation of \p this.
    public native String ascii_dump();

    //! Returns a string representation of \p this.
    public native String toString();
}
