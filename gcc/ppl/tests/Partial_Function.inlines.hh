/* Partial_Function class implementation: inline functions.
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

#ifndef PPL_Partial_Function_inlines_hh
#define PPL_Partial_Function_inlines_hh 1

namespace Parma_Polyhedra_Library {

namespace Test {

inline
Partial_Function::Partial_Function()
  : max(0) {
}

inline bool
Partial_Function::has_empty_codomain() const {
  return map.empty();
}

} // namespace Test

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Partial_Function_inlines_hh)
