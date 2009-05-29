/* Implementation of class Partial_Function (non-inline functions).
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

#include "ppl.hh"
#include "Partial_Function.defs.hh"
#include <stdexcept>
#include <iostream>

namespace Parma_Polyhedra_Library {

namespace Test {

void
Partial_Function::insert(dim_t x, dim_t y) {
 std::pair<Map::iterator, bool> stat = map.insert(Map::value_type(x, y));
 if (!stat.second)
   throw std::runtime_error("Partial_Function::insert(x, y) called"
			    " with `x' already in domain");
 if (y > max)
   max = y;
}

Partial_Function::dim_t
Partial_Function::max_in_codomain() const {
  if (has_empty_codomain())
    throw std::runtime_error("Partial_Function::max_in_codomain() called"
			     " when has_empty_codomain()");
  return max;
}

bool
Partial_Function::maps(dim_t x, dim_t& y) const {
  if (has_empty_codomain())
    throw std::runtime_error("Partial_Function::maps() called"
			     " when has_empty_codomain()");
  Map::const_iterator i = map.find(x);
  if (i != map.end()) {
    y = (*i).second;
    return true;
  }
  else
    return false;
}

void
Partial_Function::print(std::ostream& s) const {
  using namespace Parma_Polyhedra_Library::IO_Operators;

  if (has_empty_codomain())
    s << "empty" << std::endl;
  else
    for (Map::const_iterator i = map.begin(),
	   map_end = map.end(); i != map_end; ++i)
      s << Variable((*i).first) << " --> "
	<< Variable((*i).second)
	<< std::endl;
}

} // namespace Test

} // namespace Parma_Polyhedra_Library
