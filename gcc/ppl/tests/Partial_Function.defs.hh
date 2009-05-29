/* Partial_Function class declaration.
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

#ifndef PPL_Partial_Function_defs_hh
#define PPL_Partial_Function_defs_hh 1

#include "Partial_Function.types.hh"
#include <cstddef>
#include <map>
#include <iosfwd>

namespace Parma_Polyhedra_Library {

namespace Test {

class Partial_Function {
private:
  typedef size_t dim_t;

public:
  Partial_Function();
  bool has_empty_codomain() const;
  dim_t max_in_codomain() const;
  bool maps(dim_t i, dim_t& j) const;

  void print(std::ostream& s) const;

  void insert(dim_t x, dim_t y);

private:
  typedef std::map<dim_t, dim_t, std::less<dim_t> > Map;
  Map map;
  dim_t max;
};

} // namespace Test

} // namespace Parma_Polyhedra_Library

#include "Partial_Function.inlines.hh"

#endif // !defined(PPL_Partial_Function_defs_hh)
