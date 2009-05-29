/* Grid class implementation: inline functions.
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

#ifndef PPL_Grid_templates_hh
#define PPL_Grid_templates_hh 1

#include "Grid_Generator.defs.hh"
#include "Grid_Generator_System.defs.hh"
#include "Grid_Generator_System.inlines.hh"
#include <algorithm>
#include <deque>

namespace Parma_Polyhedra_Library {

template <typename Interval>
Grid::Grid(const Box<Interval>& box,
           Complexity_Class)
  : con_sys(),
    gen_sys() {
  if (box.space_dimension() > max_space_dimension())
    throw_space_dimension_overflow("Grid(box, from_bounding_box)",
				   "the space dimension of box "
				   "exceeds the maximum allowed "
				   "space dimension");

  space_dim = box.space_dimension();

  if (box.is_empty()) {
    // Empty grid.
    set_empty();
    assert(OK());
    return;
  }

  if (space_dim == 0)
    set_zero_dim_univ();
  else {
    // Initialize the space dimension as indicated by the box.
    con_sys.increase_space_dimension(space_dim);
    // Add congruences and generators according to `box'.
    PPL_DIRTY_TEMP_COEFFICIENT(l_n);
    PPL_DIRTY_TEMP_COEFFICIENT(l_d);
    PPL_DIRTY_TEMP_COEFFICIENT(u_n);
    PPL_DIRTY_TEMP_COEFFICIENT(u_d);
    gen_sys.insert(grid_point(0*Variable(space_dim-1)));
    Grid_Generator& point = gen_sys[0];
    for (dimension_type k = space_dim; k-- > 0; ) {
      bool closed = false;
      // TODO: Consider producing the system(s) in minimized form.
      if (box.get_lower_bound(k, closed, l_n, l_d)) {
	if (box.get_upper_bound(k, closed, u_n, u_d))
	  if (l_n * u_d == u_n * l_d) {
	    // A point interval sets dimension k of every point to a
	    // single value.
	    con_sys.insert(l_d * Variable(k) == l_n);

	    // Scale the point to use as divisor the lcm of the
	    // divisors of the existing point and the lower bound.
	    const Coefficient& point_divisor = point.divisor();
	    gcd_assign(u_n, l_d, point_divisor);
	    // `u_n' now holds the gcd.
	    exact_div_assign(u_n, point_divisor, u_n);
	    if (l_d < 0)
	      neg_assign(u_n);
	    // l_d * u_n == abs(l_d * (point_divisor / gcd(l_d, point_divisor)))
	    point.scale_to_divisor(l_d * u_n);
	    // Set dimension k of the point to the lower bound.
	    if (l_d < 0)
	      neg_assign(u_n);
	    // point[k + 1] = l_n * point_divisor / gcd(l_d, point_divisor)
	    point[k + 1] = l_n * u_n;

	    continue;
	  }
      }
      // A universe interval allows any value in dimension k.
      gen_sys.insert(grid_line(Variable(k)));
    }
    set_congruences_up_to_date();
    set_generators_up_to_date();
    gen_sys.unset_pending_rows();
    gen_sys.set_sorted(false);
  }

  assert(OK());
}

template <typename Box>
Grid::Grid(const Box& box, From_Covering_Box)
  : con_sys(),
    gen_sys() {

  if (box.space_dimension() > max_space_dimension())
    throw_space_dimension_overflow("Grid(box, from_covering_box)",
				   "the space dimension of box "
				   "exceeds the maximum allowed "
				   "space dimension");

  space_dim = box.space_dimension();

  PPL_DIRTY_TEMP_COEFFICIENT(l_n);
  PPL_DIRTY_TEMP_COEFFICIENT(l_d);

  // FIXME: consider whether or not covering boxes supporting open
  // boundaries should be disabled at compile time. If that is not
  // the case, consider if the test !box.is_topologically_closed()
  // can replace the whole loop here below.

  // Check that no finite bounds is open. This check must be done before
  // the empty test below, as an open bound might mean an empty box.
  for (dimension_type k = space_dim; k-- > 0; ) {
    bool closed = false;
    if (box.get_lower_bound(k, closed, l_n, l_d) && !closed)
      throw_invalid_argument("Grid(box, from_covering_box)", "box");
    if (box.get_upper_bound(k, closed, l_n, l_d) && !closed)
      throw_invalid_argument("Grid(box, from_covering_box)", "box");
  }

  if (box.is_empty()) {
    // Empty grid.
    set_empty();
    assert(OK());
    return;
  }

  if (space_dim == 0)
    set_zero_dim_univ();
  else {
    // Initialize the space dimension as indicated by the box.
    con_sys.increase_space_dimension(space_dim);
    // Add congruences according to `box'.
    PPL_DIRTY_TEMP_COEFFICIENT(u_n);
    PPL_DIRTY_TEMP_COEFFICIENT(u_d);
    PPL_DIRTY_TEMP_COEFFICIENT(d);
    gen_sys.insert(grid_point(0*Variable(space_dim-1)));
    Grid_Generator& point = gen_sys[0];
    for (dimension_type k = space_dim; k-- > 0; ) {
      bool closed = false;
      // TODO: Consider producing the system(s) in minimized form.
      if (box.get_lower_bound(k, closed, l_n, l_d)) {

	const Coefficient& point_divisor = point.divisor();
	assert(l_d > 0);
	assert(point_divisor > 0);
	// Use `d' to hold the gcd.
	gcd_assign(d, l_d, point_divisor);
	// Scale the point to use as divisor the lcm of the existing
	// point divisor and the divisor of the lower bound.
	exact_div_assign(d, point_divisor, d);
	// l_d * d == abs(l_d) * (point_divisor / gcd(l_d, point_divisor))
	point.scale_to_divisor(l_d * d);
	// Set dimension k of the point to the lower bound.
	// point[k + 1] = l_n * (point_divisor / gcd(l_d, point_divisor))
	point[k + 1] = l_n * d;

	if (box.get_upper_bound(k, closed, u_n, u_d)) {
	  if (l_n * u_d == u_n * l_d) {
	    // A point interval allows any point along the dimension
	    // k axis.
	    gen_sys.insert(grid_line(Variable(k)));
	    continue;
	  }
	  assert(l_d > 0);
	  assert(u_d > 0);
	  gcd_assign(d, l_d, u_d);
	  // `d' is the gcd of the divisors.
	  exact_div_assign(l_d, l_d, d);
	  exact_div_assign(d, u_d, d);
	  l_n *= d;
	  // `l_d' is now the smallest integer expression of the size of
	  // the original l_d relative to u_d.
	  u_n = (u_n * l_d) - l_n;
	  // `u_n' is now the distance between u_n and l_n (given a
	  // divisor of lcm of l_d and u_d.
	  l_d *= u_d;
	  // `l_d' is now the lcm of the divisors.
	  con_sys.insert((l_d * Variable(k) %= l_n) / u_n);
	  gen_sys.insert(parameter(u_n * Variable(k), l_d));
	}
	else
	  // An interval bounded only from below produces an
	  // equality.
	  con_sys.insert(l_d * Variable(k) == l_n);
      }
      else
	if (box.get_upper_bound(k, closed, u_n, u_d)) {
	  const Coefficient& point_divisor = point.divisor();
	  assert(u_d > 0);
	  assert(point_divisor > 0);
	  // Use `d' to hold the gcd.
	  gcd_assign(d, u_d, point_divisor);
	  // Scale the point to use as divisor the lcm of the existing
	  // point divisor and the divisor of the lower bound.
	  exact_div_assign(d, point_divisor, d);
	  // u_d * d == abs(u_d) * (point_divisor / gcd(u_d, point_divisor))
	  point.scale_to_divisor(u_d * d);
	  // Set dimension k of the point to the lower bound.
	  // point[k + 1] = u_n * (point_divisor / gcd(u_d, point_divisor))
	  point[k + 1] = u_n * d;

	  // An interval bounded only from above produces an equality.
	  con_sys.insert(u_d * Variable(k) == u_n);
	}
	else {
	  // Any universe interval produces an empty grid.
	  set_empty();
	  assert(OK());
	  return;
	}
    }
    normalize_divisors(gen_sys);
    set_congruences_up_to_date();
    set_generators_up_to_date();
    gen_sys.set_sorted(false);
    gen_sys.unset_pending_rows();
  }

  assert(OK());
}

template <typename Interval>
void
Grid::get_covering_box(Box<Interval>& box) const {
  // Dimension-compatibility check.
  if (space_dim > box.space_dimension())
    throw_dimension_incompatible("get_covering_box(box)", "box",
				 box.space_dimension());

  Box<Interval> new_box(box.space_dimension());

  if (marked_empty()) {
    box = new_box;
    box.set_empty();
    return;
  }
  if (space_dim == 0) {
    return;
  }
  if (!generators_are_up_to_date() && !update_generators()) {
    // Updating found the grid empty.
    box = new_box;
    box.set_empty();
    return;
  }

  assert(!gen_sys.has_no_rows());

  dimension_type num_dims = gen_sys.num_columns() - 2 /* parameter divisor */;
  dimension_type num_rows = gen_sys.num_rows();

  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  PPL_DIRTY_TEMP_COEFFICIENT(bound);

  if (num_rows > 1) {
    Row interval_sizes(num_dims, Row::Flags());
    std::vector<bool> interval_emptiness(num_dims, false);

    // Store in `interval_sizes', for each column (that is, for each
    // dimension), the GCD of all the values in that column where the
    // row is of type parameter.

    for (dimension_type dim = num_dims; dim-- > 0; )
      interval_sizes[dim] = 0;
    const Grid_Generator *first_point = NULL;
    for (dimension_type row = 0; row < num_rows; ++row) {
      Grid_Generator& gen = const_cast<Grid_Generator&>(gen_sys[row]);
      if (gen.is_line()) {
	for (dimension_type dim = 0; dim < num_dims; ++dim)
	  if (!interval_emptiness[dim] && gen[dim+1] != 0) {
	    // Empty interval, set both bounds for associated
      	    // dimension to zero.
            new_box.add_constraint(Variable(dim) == 0);
	    interval_emptiness[dim] = true;
	  }
	continue;
      }
      if (gen.is_point()) {
	if (first_point == NULL) {
	  first_point = &gen_sys[row];
	  continue;
	}
	const Grid_Generator& point = *first_point;
	// Convert the point `gen' to a parameter.
	dimension_type dim = num_dims;
	do {
	  gen[dim] -= point[dim];
	}
	while (dim-- > 0);
	gen.set_divisor(point.divisor());
      }
      for (dimension_type dim = num_dims; dim-- > 0; )
	if (!interval_emptiness[dim])
	  gcd_assign(interval_sizes[dim], interval_sizes[dim], gen[dim+1]);
    }

    // For each dimension set the lower bound of the interval to the
    // grid value closest to the origin, and the upper bound to the
    // addition of the lower bound and the shortest distance in the
    // given dimension between any two grid points.
    const Grid_Generator& point = *first_point;
    const Coefficient& divisor = point.divisor();
    PPL_DIRTY_TEMP_COEFFICIENT(lower_bound);
    for (dimension_type dim = num_dims; dim-- > 0; ) {
      if (interval_emptiness[dim])
	continue;

      lower_bound = point[dim+1];

      // If the interval size is zero then all points have the same
      // value in this dimension, so set only the lower bound.
      if (interval_sizes[dim] != 0) {
	// Make the lower bound as close as possible to the origin,
	// leaving the sign the same.
	lower_bound %= interval_sizes[dim];
	// Check if the lowest value the other side of the origin is
	// closer to the origin, preferring the lowest positive if they
	// are equal.
	if (lower_bound > 0) {
	  if (interval_sizes[dim] - lower_bound < lower_bound)
	    lower_bound -= interval_sizes[dim];
	}
	else if (lower_bound < 0
		 && interval_sizes[dim] + lower_bound < - lower_bound)
	  lower_bound += interval_sizes[dim];

	// Reduce the bound fraction first.
	bound = interval_sizes[dim] + lower_bound;
	gcd_assign(gcd, bound, divisor);
	exact_div_assign(bound, bound, gcd);
	exact_div_assign(gcd, divisor, gcd);
	// `gcd' now holds the reduced divisor.
        new_box.add_constraint(gcd*Variable(dim) <= bound);
      }

      // Reduce the bound fraction first.
      gcd_assign(gcd, lower_bound, divisor);
      exact_div_assign(lower_bound, lower_bound, gcd);
      exact_div_assign(gcd, divisor, gcd);
      // `gcd' now holds the reduced divisor.
      new_box.add_constraint(gcd*Variable(dim) >= lower_bound);
    }
  }
  else {
    const Grid_Generator& point = gen_sys[0];
    const Coefficient& divisor = point.divisor();
    // The covering box of a single point has only lower bounds.
    for (dimension_type dim = num_dims; dim-- > 0; ) {
      // Reduce the bound fraction first.
      gcd_assign(gcd, point[dim+1], divisor);
      exact_div_assign(bound, point[dim+1], gcd);
      exact_div_assign(gcd, divisor, gcd);
      // `gcd' now holds the reduced divisor.
      new_box.add_constraint(gcd*Variable(dim) >= bound);
    }
  }

  box.swap(new_box);
}

template <typename Partial_Function>
void
Grid::map_space_dimensions(const Partial_Function& pfunc) {
  if (space_dim == 0)
    return;

  if (pfunc.has_empty_codomain()) {
    // All dimensions vanish: the grid becomes zero_dimensional.
    if (marked_empty()
	|| (!generators_are_up_to_date() && !update_generators())) {
      // Removing all dimensions from the empty grid.
      space_dim = 0;
      set_empty();
    }
    else
      // Removing all dimensions from a non-empty grid.
      set_zero_dim_univ();

    assert(OK());
    return;
  }

  dimension_type new_space_dimension = pfunc.max_in_codomain() + 1;

  if (new_space_dimension == space_dim) {
    // The partial function `pfunc' is indeed total and thus specifies
    // a permutation, that is, a renaming of the dimensions.  For
    // maximum efficiency, we will simply permute the columns of the
    // constraint system and/or the generator system.

    // We first compute suitable permutation cycles for the columns of
    // the `con_sys' and `gen_sys' matrices.  We will represent them
    // with a linear array, using 0 as a terminator for each cycle
    // (notice that the columns with index 0 of `con_sys' and
    // `gen_sys' represent the inhomogeneous terms, and thus are
    // unaffected by the permutation of dimensions).
    // Cycles of length 1 will be omitted so that, in the worst case,
    // we will have `space_dim' elements organized in `space_dim/2'
    // cycles, which means we will have at most `space_dim/2'
    // terminators.
    std::vector<dimension_type> cycles;
    cycles.reserve(space_dim + space_dim/2);

    // Used to mark elements as soon as they are inserted in a cycle.
    std::deque<bool> visited(space_dim);

    for (dimension_type i = space_dim; i-- > 0; ) {
      if (!visited[i]) {
	dimension_type j = i;
	do {
	  visited[j] = true;
	  // The following initialization is only to make the compiler happy.
	  dimension_type k = 0;
	  if (!pfunc.maps(j, k))
	    throw_invalid_argument("map_space_dimensions(pfunc)",
				   " pfunc is inconsistent");
	  if (k == j)
	    // Cycle of length 1: skip it.
	    goto skip;

	  cycles.push_back(j+1);
	  // Go along the cycle.
	  j = k;
	} while (!visited[j]);
	// End of cycle: mark it.
	cycles.push_back(0);
      skip:
	;
      }
    }

    // If `cycles' is empty then `pfunc' is the identity.
    if (cycles.empty())
      return;

    // Permute all that is up-to-date.
    if (congruences_are_up_to_date()) {
      con_sys.permute_columns(cycles);
      clear_congruences_minimized();
    }

    if (generators_are_up_to_date()) {
      gen_sys.permute_columns(cycles);
      clear_generators_minimized();
    }

    assert(OK());
    return;
  }

  // If control gets here, then `pfunc' is not a permutation and some
  // dimensions must be projected away.

  const Grid_Generator_System& old_gensys = grid_generators();

  if (old_gensys.has_no_rows()) {
    // The grid is empty.
    Grid new_grid(new_space_dimension, EMPTY);
    std::swap(*this, new_grid);
    assert(OK());
    return;
  }

  // Make a local copy of the partial function.
  std::vector<dimension_type> pfunc_maps(space_dim, not_a_dimension());
  for (dimension_type j = space_dim; j-- > 0; ) {
    dimension_type pfunc_j;
    if (pfunc.maps(j, pfunc_j))
      pfunc_maps[j] = pfunc_j;
  }

  Grid_Generator_System new_gensys;
  // Set sortedness, for the assertion met via gs::insert.
  new_gensys.set_sorted(false);
  // Get the divisor of the first point.
  Grid_Generator_System::const_iterator i;
  Grid_Generator_System::const_iterator old_gensys_end = old_gensys.end();
  for (i = old_gensys.begin(); i != old_gensys_end; ++i)
    if (i->is_point())
      break;
  assert(i != old_gensys_end);
  const Coefficient& system_divisor = i->divisor();
  for (i = old_gensys.begin(); i != old_gensys_end; ++i) {
    const Grid_Generator& old_g = *i;
    Linear_Expression e(0 * Variable(new_space_dimension-1));
    bool all_zeroes = true;
    for (dimension_type j = space_dim; j-- > 0; ) {
      if (old_g.coefficient(Variable(j)) != 0
	  && pfunc_maps[j] != not_a_dimension()) {
	e += Variable(pfunc_maps[j]) * old_g.coefficient(Variable(j));
	all_zeroes = false;
      }
    }
    switch (old_g.type()) {
    case Grid_Generator::LINE:
      if (!all_zeroes)
	new_gensys.insert(grid_line(e));
      break;
    case Grid_Generator::PARAMETER:
      if (!all_zeroes)
	new_gensys.insert(parameter(e, system_divisor));
      break;
    case Grid_Generator::POINT:
      new_gensys.insert(grid_point(e, old_g.divisor()));
      break;
    case Grid_Generator::CLOSURE_POINT:
    default:
      assert(0);
    }
  }

  Grid new_grid(new_gensys);
  std::swap(*this, new_grid);

  assert(OK(true));
}

// Needed for converting the congruence or grid_generator system
// to "strong minimal form".
template <typename M, typename R>
void
Grid::reduce_reduced(M& sys,
		     const dimension_type dim,
		     const dimension_type pivot_index,
		     const dimension_type start,
		     const dimension_type end,
		     const Dimension_Kinds& dim_kinds,
		     const bool generators) {
  R& pivot = sys[pivot_index];

  const Coefficient& pivot_dim = pivot[dim];

  if (pivot_dim == 0)
    return;

  PPL_DIRTY_TEMP_COEFFICIENT(pivot_dim_half);
  pivot_dim_half = (pivot_dim + 1) / 2;
  Dimension_Kind row_kind = dim_kinds[dim];
  Dimension_Kind line_or_equality, virtual_kind;
  int jump;
  if (generators) {
    line_or_equality = LINE;
    virtual_kind = GEN_VIRTUAL;
    jump = -1;
  }
  else {
    line_or_equality = EQUALITY;
    virtual_kind = CON_VIRTUAL;
    jump = 1;
  }

  PPL_DIRTY_TEMP_COEFFICIENT(num_rows_to_subtract);
  PPL_DIRTY_TEMP_COEFFICIENT(row_dim_remainder);
  for (dimension_type row_index = pivot_index, kinds_index = dim + jump;
       row_index-- > 0;
       kinds_index += jump) {
    // Move over any virtual rows.
    while (dim_kinds[kinds_index] == virtual_kind)
      kinds_index += jump;

    // row_kind CONGRUENCE is included as PARAMETER
    if (row_kind == line_or_equality
	|| (row_kind == PARAMETER
	    && dim_kinds[kinds_index] == PARAMETER)) {
      R& row = sys[row_index];

      const Coefficient& row_dim = row[dim];
      // num_rows_to_subtract may be positive or negative.
      num_rows_to_subtract = row_dim / pivot_dim;

      // Ensure that after subtracting num_rows_to_subtract * r_dim
      // from row_dim, -pivot_dim_half < row_dim <= pivot_dim_half.
      // E.g., if pivot[dim] = 9, then after this reduction
      // -5 < row_dim <= 5.
      row_dim_remainder = row_dim % pivot_dim;
      if (row_dim_remainder < 0) {
	if (row_dim_remainder <= -pivot_dim_half)
	  --num_rows_to_subtract;
      }
      else if (row_dim_remainder > 0 && row_dim_remainder > pivot_dim_half)
	++num_rows_to_subtract;

      // Subtract num_rows_to_subtract copies of pivot from row i.  Only the
      // entries from dim need to be subtracted, as the preceding
      // entries are all zero.
      // If num_rows_to_subtract is negative, these copies of pivot are
      // added to row i.
      if (num_rows_to_subtract != 0)
	for (dimension_type col = start; col <= end; ++col)
	  sub_mul_assign(row[col], num_rows_to_subtract, pivot[col]);
    }
  }
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Grid_templates_hh)
