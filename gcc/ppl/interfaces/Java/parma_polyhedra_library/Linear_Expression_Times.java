/* Linear_Expression_Times class definition and implementation.
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

//! The product of a linear expression and a coefficient.
/*! \ingroup PPL_Java_interface */
public class Linear_Expression_Times
    extends Linear_Expression {

    //! The value of the left hand side of \p this.
    protected Linear_Expression_Coefficient lhs;

    //! The value of the left hand side of \p this.
    protected Linear_Expression rhs;

    //! Builds an object cloning the input arguments.
    public Linear_Expression_Times(Linear_Expression l, Coefficient c) {
	lhs = new Linear_Expression_Coefficient(new Coefficient(c));
	rhs = l.clone();
    }

    //! Returns the left hand side of \p this.
   public Linear_Expression left_hand_side() {
	return lhs;
    }

    //! Returns the right hand side of \p this.
    public Linear_Expression right_hand_side() {
	return rhs;
    }

    //! Builds a copy of this.
    public Linear_Expression_Times clone() {
	return new Linear_Expression_Times(rhs.clone(),
					   new Coefficient(lhs.argument()));
    }
}
