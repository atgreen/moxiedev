/* Partial_Function Java interface declaration.
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

//! A partial function on space dimension indices.
/*! \ingroup PPL_Java_interface
  In order to specify how space dimensions should be mapped by methods
  named \c map_space_dimensions, the user should implement this
  interface.

  \note
  An example of implementation can be found in the PPL test file
  <CODE>interfaces/Java/tests/Test_Partial_Function.java</CODE>.
*/
public interface Partial_Function {
    /*! \brief
      Returns \c true if and only if the partial function has
      an empty codomain (i.e., it is always undefined).

      This method will always be called before the other methods
      of the interface. Moreover, if \c true is returned, then
      none of the other interface methods will be called.
    */
    boolean has_empty_codomain();

    /*! \brief
      Returns the maximum value that belongs to the codomain
      of the partial function.
    */
    long max_in_codomain();

    /*! \brief
      Sets \p j to the value (if any) of the partial function on index \p i.

      The function returns \c true if and only if the partial function
      is defined on domain value \p i.
    */
    boolean maps(Long i, By_Reference<Long> j);
}
