/* Box class implementation: non-inline template functions.
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

#ifndef PPL_Box_templates_hh
#define PPL_Box_templates_hh 1

#include "Variables_Set.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "Generator_System.defs.hh"
#include "Generator_System.inlines.hh"
#include "Poly_Con_Relation.defs.hh"
#include "Poly_Gen_Relation.defs.hh"
#include "Polyhedron.defs.hh"
#include "Grid.defs.hh"
#include "BD_Shape.defs.hh"
#include "Octagonal_Shape.defs.hh"
#include "MIP_Problem.defs.hh"
#include "Rational_Interval.hh"
#include <iostream>

namespace Parma_Polyhedra_Library {

template <typename ITV>
inline
Box<ITV>::Box(dimension_type num_dimensions, Degenerate_Element kind)
  : seq(num_dimensions <= max_space_dimension()
	? num_dimensions
	: (throw_space_dimension_overflow("Box(n, k)",
					  "n exceeds the maximum "
					  "allowed space dimension"),
	   num_dimensions)),
    status() {
  // In a box that is marked empty the intervals are completely
  // meaningless: we exploit this by avoiding their initialization.
  if (kind == UNIVERSE) {
    for (dimension_type i = num_dimensions; i-- > 0; )
      seq[i].assign(UNIVERSE);
    set_empty_up_to_date();
  }
  else
    set_empty();
  assert(OK());
}

template <typename ITV>
inline
Box<ITV>::Box(const Constraint_System& cs)
  : seq(cs.space_dimension() <= max_space_dimension()
	? cs.space_dimension()
	: (throw_space_dimension_overflow("Box(cs)",
					  "cs exceeds the maximum "
					  "allowed space dimension"),
	   cs.space_dimension())),
    status() {
  // FIXME: check whether we can avoid the double initialization.
  for (dimension_type i = cs.space_dimension(); i-- > 0; )
    seq[i].assign(UNIVERSE);
  add_constraints_no_check(cs);
}

template <typename ITV>
inline
Box<ITV>::Box(const Congruence_System& cgs)
  : seq(cgs.space_dimension() <= max_space_dimension()
	? cgs.space_dimension()
	: (throw_space_dimension_overflow("Box(cgs)",
					  "cgs exceeds the maximum "
					  "allowed space dimension"),
	   cgs.space_dimension())),
    status() {
  // FIXME: check whether we can avoid the double initialization.
  for (dimension_type i = cgs.space_dimension(); i-- > 0; )
    seq[i].assign(UNIVERSE);
  add_congruences_no_check(cgs);
}

template <typename ITV>
template <typename Other_ITV>
inline
Box<ITV>::Box(const Box<Other_ITV>& y, Complexity_Class)
  : seq(y.space_dimension()),
    // FIXME: why the following does not work?
    // status(y.status) {
    status() {
  // FIXME: remove when the above is fixed.
  if (y.marked_empty())
    set_empty();

  if (!y.marked_empty())
    for (dimension_type k = y.space_dimension(); k-- > 0; )
      seq[k].assign(y.seq[k]);
  assert(OK());
}

template <typename ITV>
Box<ITV>::Box(const Generator_System& gs)
  : seq(gs.space_dimension() <= max_space_dimension()
	? gs.space_dimension()
	: (throw_space_dimension_overflow("Box(gs)",
					  "gs exceeds the maximum "
					  "allowed space dimension"),
	   gs.space_dimension())),
    status() {
  const Generator_System::const_iterator gs_begin = gs.begin();
  const Generator_System::const_iterator gs_end = gs.end();
  if (gs_begin == gs_end) {
    // An empty generator system defines the empty box.
    set_empty();
    return;
  }

  // The empty flag will be meaningful, whatever happens from now on.
  set_empty_up_to_date();

  const dimension_type space_dim = space_dimension();
  PPL_DIRTY_TEMP0(mpq_class, q);
  bool point_seen = false;
  // Going through all the points.
  for (Generator_System::const_iterator
	 gs_i = gs_begin; gs_i != gs_end; ++gs_i) {
    const Generator& g = *gs_i;
    if (g.is_point()) {
      const Coefficient& d = g.divisor();
      if (point_seen) {
	// This is not the first point: `seq' already contains valid values.
	for (dimension_type i = space_dim; i-- > 0; ) {
	  assign_r(q.get_num(), g.coefficient(Variable(i)), ROUND_NOT_NEEDED);
	  assign_r(q.get_den(), d, ROUND_NOT_NEEDED);
	  q.canonicalize();
	  seq[i].join_assign(q);
	}
      }
      else {
	// This is the first point seen: initialize `seq'.
	point_seen = true;
	for (dimension_type i = space_dim; i-- > 0; ) {
	  assign_r(q.get_num(), g.coefficient(Variable(i)), ROUND_NOT_NEEDED);
	  assign_r(q.get_den(), d, ROUND_NOT_NEEDED);
	  q.canonicalize();
	  seq[i].assign(q);
	}
      }
    }
  }

  if (!point_seen)
    // The generator system is not empty, but contains no points.
    throw std::invalid_argument("PPL::Box<ITV>::Box(gs):\n"
				"the non-empty generator system gs "
				"contains no points.");

  // Going through all the lines, rays and closure points.
  ITV q_interval;
  for (Generator_System::const_iterator gs_i = gs_begin;
       gs_i != gs_end; ++gs_i) {
    const Generator& g = *gs_i;
    switch (g.type()) {
    case Generator::LINE:
      for (dimension_type i = space_dim; i-- > 0; )
	if (g.coefficient(Variable(i)) != 0)
	  seq[i].assign(UNIVERSE);
      break;
    case Generator::RAY:
      for (dimension_type i = space_dim; i-- > 0; )
	switch (sgn(g.coefficient(Variable(i)))) {
	case 1:
	  seq[i].upper_set(UNBOUNDED);
	  break;
	case -1:
	  seq[i].lower_set(UNBOUNDED);
	  break;
	default:
	  break;
	}
      break;
    case Generator::CLOSURE_POINT:
      {
	const Coefficient& d = g.divisor();
	for (dimension_type i = space_dim; i-- > 0; ) {
	  assign_r(q.get_num(), g.coefficient(Variable(i)), ROUND_NOT_NEEDED);
	  assign_r(q.get_den(), d, ROUND_NOT_NEEDED);
	  q.canonicalize();
	  ITV& seq_i = seq[i];
	  seq_i.lower_widen(q, true);
	  seq_i.upper_widen(q, true);
	}
      }
      break;
    default:
      // Points already dealt with.
      break;
    }
  }
  assert(OK());
}

template <typename ITV>
template <typename T>
Box<ITV>::Box(const BD_Shape<T>& bds, Complexity_Class)
  : seq(bds.space_dimension() <= max_space_dimension()
	? bds.space_dimension()
	: (throw_space_dimension_overflow("Box(bds)",
					  "bds exceeds the maximum "
					  "allowed space dimension"),
	   bds.space_dimension())),
    status() {
  // Expose all the interval constraints.
  bds.shortest_path_closure_assign();
  if (bds.marked_empty()) {
    set_empty();
    assert(OK());
    return;
  }

  // The empty flag will be meaningful, whatever happens from now on.
  set_empty_up_to_date();

  const dimension_type space_dim = space_dimension();
  if (space_dim == 0) {
    assert(OK());
    return;
  }

  PPL_DIRTY_TEMP(typename BD_Shape<T>::coefficient_type, tmp);
  const DB_Row<typename BD_Shape<T>::coefficient_type>& dbm_0 = bds.dbm[0];
  for (dimension_type i = space_dim; i-- > 0; ) {
    ITV& seq_i = seq[i];
    // Set the upper bound.
    const typename BD_Shape<T>::coefficient_type& u = dbm_0[i+1];
    if (is_plus_infinity(u))
      seq_i.upper_set_uninit(UNBOUNDED);
    else
      seq_i.upper_set_uninit(u);

    // Set the lower bound.
    const typename BD_Shape<T>::coefficient_type& negated_l = bds.dbm[i+1][0];
    if (is_plus_infinity(negated_l))
      seq_i.lower_set_uninit(UNBOUNDED);
    else {
      neg_assign_r(tmp, negated_l, ROUND_DOWN);
      seq_i.lower_set_uninit(tmp);
    }

    // Complete the interval initialization.
    seq_i.complete_init();
  }
  assert(OK());
}

template <typename ITV>
template <typename T>
Box<ITV>::Box(const Octagonal_Shape<T>& oct, Complexity_Class)
  : seq(oct.space_dimension() <= max_space_dimension()
	? oct.space_dimension()
	: (throw_space_dimension_overflow("Box(oct)",
					  "oct exceeds the maximum "
					  "allowed space dimension"),
	   oct.space_dimension())),
    status() {
  // Expose all the interval constraints.
  oct.strong_closure_assign();
  if (oct.marked_empty()) {
    set_empty();
    return;
  }

  // The empty flag will be meaningful, whatever happens from now on.
  set_empty_up_to_date();

  const dimension_type space_dim = space_dimension();
  if (space_dim == 0)
    return;

  PPL_DIRTY_TEMP0(mpq_class, bound);
  for (dimension_type i = space_dim; i-- > 0; ) {
    ITV& seq_i = seq[i];
    const dimension_type ii = 2*i;
    const dimension_type cii = ii + 1;

    // Set the upper bound.
    const typename Octagonal_Shape<T>::coefficient_type& twice_ub
      = oct.matrix[cii][ii];
    if (!is_plus_infinity(twice_ub)) {
      assign_r(bound, twice_ub, ROUND_NOT_NEEDED);
      div2exp_assign_r(bound, bound, 1, ROUND_NOT_NEEDED);
      seq_i.upper_set_uninit(bound);
    }
    else
      seq_i.upper_set_uninit(UNBOUNDED);

    // Set the lower bound.
    const typename Octagonal_Shape<T>::coefficient_type& twice_lb
      = oct.matrix[ii][cii];
    if (!is_plus_infinity(twice_lb)) {
      assign_r(bound, twice_lb, ROUND_NOT_NEEDED);
      neg_assign_r(bound, bound, ROUND_NOT_NEEDED);
      div2exp_assign_r(bound, bound, 1, ROUND_NOT_NEEDED);
      seq_i.lower_set_uninit(bound);
    }
    else
      seq_i.lower_set_uninit(UNBOUNDED);
    seq_i.complete_init();
  }
}

template <typename ITV>
Box<ITV>::Box(const Polyhedron& ph, Complexity_Class complexity)
  : seq(ph.space_dimension() <= max_space_dimension()
	? ph.space_dimension()
	: (throw_space_dimension_overflow("Box(ph)",
					  "ph exceeds the maximum "
					  "allowed space dimension"),
	   ph.space_dimension())),
    status() {
  // The empty flag will be meaningful, whatever happens from now on.
  set_empty_up_to_date();

  // We do not need to bother about `complexity' if:
  // a) the polyhedron is already marked empty; or ...
  if (ph.marked_empty()) {
    set_empty();
    return;
  }

  // b) the polyhedron is zero-dimensional; or ...
  const dimension_type space_dim = ph.space_dimension();
  if (space_dim == 0)
    return;

  // c) the polyhedron is already described by a generator system.
  if (ph.generators_are_up_to_date() && !ph.has_pending_constraints()) {
    Box tmp(ph.generators());
    swap(tmp);
    return;
  }

  // Here generators are not up-to-date or there are pending constraints.
  assert(ph.constraints_are_up_to_date());

  if (complexity == POLYNOMIAL_COMPLEXITY) {
    // FIXME: is there a way to avoid this initialization?
    for (dimension_type i = space_dimension(); i-- > 0; )
      seq[i].assign(UNIVERSE);
    // Extract easy-to-find bounds from constraints.
    refine_with_constraints(ph.simplified_constraints());
  }
  else if (complexity == SIMPLEX_COMPLEXITY) {
    MIP_Problem lp(space_dim);
    const Constraint_System& ph_cs = ph.constraints();
    if (!ph_cs.has_strict_inequalities())
      lp.add_constraints(ph_cs);
    else
      // Adding to `lp' a topologically closed version of `ph_cs'.
      for (Constraint_System::const_iterator i = ph_cs.begin(),
	     ph_cs_end = ph_cs.end(); i != ph_cs_end; ++i) {
	const Constraint& c = *i;
	if (c.is_strict_inequality())
	  lp.add_constraint(Linear_Expression(c) >= 0);
	else
	  lp.add_constraint(c);
      }
    // Check for unsatisfiability.
    if (!lp.is_satisfiable()) {
      set_empty();
      return;
    }
    // Get all the bounds for the space dimensions.
    Generator g(point());
    PPL_DIRTY_TEMP0(mpq_class, bound);
    PPL_DIRTY_TEMP(Coefficient, bound_num);
    PPL_DIRTY_TEMP(Coefficient, bound_den);
    for (dimension_type i = space_dim; i-- > 0; ) {
      ITV& seq_i = seq[i];
      lp.set_objective_function(Variable(i));
      // Evaluate upper bound.
      lp.set_optimization_mode(MAXIMIZATION);
      if (lp.solve() == OPTIMIZED_MIP_PROBLEM) {
	g = lp.optimizing_point();
	lp.evaluate_objective_function(g, bound_num, bound_den);
	assign_r(bound.get_num(), bound_num, ROUND_NOT_NEEDED);
	assign_r(bound.get_den(), bound_den, ROUND_NOT_NEEDED);
	assert(is_canonical(bound));
	seq_i.upper_set_uninit(bound);
      }
      else
	seq_i.upper_set_uninit(UNBOUNDED);
      // Evaluate optimal lower bound.
      lp.set_optimization_mode(MINIMIZATION);
      if (lp.solve() == OPTIMIZED_MIP_PROBLEM) {
	g = lp.optimizing_point();
	lp.evaluate_objective_function(g, bound_num, bound_den);
	assign_r(bound.get_num(), bound_num, ROUND_NOT_NEEDED);
	assign_r(bound.get_den(), bound_den, ROUND_NOT_NEEDED);
	assert(is_canonical(bound));
	seq_i.lower_set_uninit(bound);
      }
      else
	seq_i.lower_set_uninit(UNBOUNDED);
      seq_i.complete_init();
    }
  }
  else {
    assert(complexity == ANY_COMPLEXITY);
    if (ph.is_empty())
      set_empty();
    else {
      Box tmp(ph.generators());
      swap(tmp);
    }
  }
}

template <typename ITV>
Box<ITV>::Box(const Grid& gr, Complexity_Class)
  : seq(gr.space_dimension() <= max_space_dimension()
	? gr.space_dimension()
	: (throw_space_dimension_overflow("Box(gr)",
					  "gr exceeds the maximum "
					  "allowed space dimension"),
	   gr.space_dimension())),
    status() {

  // FIXME: here we are not taking advantage of intervals with restrictions!

  if (gr.marked_empty()) {
    set_empty();
    return;
  }

  // The empty flag will be meaningful, whatever happens from now on.
  set_empty_up_to_date();

  const dimension_type space_dim = gr.space_dimension();

  if (space_dim == 0)
    return;

  if (!gr.generators_are_up_to_date() && !gr.update_generators()) {
    // Updating found the grid empty.
    set_empty();
    return;
  }

  assert(!gr.gen_sys.empty());

  // For each dimension that is bounded by the grid, set both bounds
  // of the interval to the value of the associated coefficient in a
  // generator point.
  PPL_DIRTY_TEMP0(mpq_class, bound);
  PPL_DIRTY_TEMP(Coefficient, bound_num);
  PPL_DIRTY_TEMP(Coefficient, bound_den);
  for (dimension_type i = space_dim; i-- > 0; ) {
    ITV& seq_i = seq[i];
    Variable var(i);
    bool max;
    if (gr.maximize(var, bound_num, bound_den, max)) {
      assign_r(bound.get_num(), bound_num, ROUND_NOT_NEEDED);
      assign_r(bound.get_den(), bound_den, ROUND_NOT_NEEDED);
      bound.canonicalize();
      seq_i.assign(bound);
    }
    else
      seq_i.assign(UNIVERSE);
  }
}

template <typename ITV>
template <typename D1, typename D2, typename R>
Box<ITV>::Box(const Partially_Reduced_Product<D1, D2, R>& dp,
              Complexity_Class complexity)
  : seq(), status() {
  if (dp.space_dimension() > max_space_dimension())
    throw_space_dimension_overflow("Box(dp)",
                                   "dp exceeds the maximum "
                                   "allowed space dimension");
  Box tmp1(dp.domain1(), complexity);
  Box tmp2(dp.domain2(), complexity);
  tmp1.intersection_assign(tmp2);
  swap(tmp1);
}

template <typename ITV>
inline void
Box<ITV>::add_space_dimensions_and_embed(const dimension_type m) {
  // Adding no dimensions is a no-op.
  if (m == 0)
    return;
  // To embed an n-dimension space box in a (n+m)-dimension space,
  // we just add `m' new universe elements to the sequence.
  seq.insert(seq.end(), m, ITV(UNIVERSE));
  assert(OK());
}

template <typename ITV>
inline void
Box<ITV>::add_space_dimensions_and_project(const dimension_type m) {
  // Adding no dimensions is a no-op.
  if (m == 0)
    return;
  // Add `m' new zero elements to the sequence.
  seq.insert(seq.end(), m, ITV(0));
  assert(OK());
}

template <typename ITV>
bool
operator==(const Box<ITV>& x, const Box<ITV>& y) {
  const dimension_type x_space_dim = x.space_dimension();
  if (x_space_dim != y.space_dimension())
    return false;

  if (x.is_empty())
    return y.is_empty();

  if (y.is_empty())
    return x.is_empty();

  for (dimension_type k = x_space_dim; k-- > 0; )
    if (x.seq[k] != y.seq[k])
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::bounds(const Linear_Expression& expr, const bool from_above) const {
  // `expr' should be dimension-compatible with `*this'.
  const dimension_type expr_space_dim = expr.space_dimension();
  const dimension_type space_dim = space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible((from_above
				  ? "bounds_from_above(e)"
				  : "bounds_from_below(e)"), "e", expr);
  // A zero-dimensional or empty Box bounds everything.
  if (space_dim == 0 || is_empty())
    return true;

  const int from_above_sign = from_above ? 1 : -1;
  for (dimension_type i = expr_space_dim; i-- > 0; )
    switch (sgn(expr.coefficient(Variable(i))) * from_above_sign) {
    case 1:
      if (seq[i].upper_is_unbounded())
	return false;
      break;
    case 0:
      // Nothing to do.
      break;
    case -1:
      if (seq[i].lower_is_unbounded())
	return false;
      break;
    }
  return true;
}

template <typename ITV>
Poly_Con_Relation
interval_relation(const ITV& i,
		  const Constraint::Type constraint_type,
		  Coefficient_traits::const_reference num,
		  Coefficient_traits::const_reference den) {

  if (i.is_universe())
    return Poly_Con_Relation::strictly_intersects();

  PPL_DIRTY_TEMP0(mpq_class, bound);
  assign_r(bound.get_num(), num, ROUND_NOT_NEEDED);
  assign_r(bound.get_den(), den, ROUND_NOT_NEEDED);
  bound.canonicalize();
  neg_assign_r(bound, bound, ROUND_NOT_NEEDED);
  const bool is_lower_bound = (den > 0);

  PPL_DIRTY_TEMP0(mpq_class, bound_diff);
  if (constraint_type == Constraint::EQUALITY) {
    if (i.lower_is_unbounded()) {
      assert(!i.upper_is_unbounded());
      assign_r(bound_diff, i.upper(), ROUND_NOT_NEEDED);
      sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
      switch (sgn(bound_diff)) {
      case 1:
	return Poly_Con_Relation::strictly_intersects();
      case 0:
	return i.upper_is_open()
	  ? Poly_Con_Relation::is_disjoint()
	  : Poly_Con_Relation::strictly_intersects();
      case -1:
	return Poly_Con_Relation::is_disjoint();
      }
    }
    else {
      assign_r(bound_diff, i.lower(), ROUND_NOT_NEEDED);
      sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
      switch (sgn(bound_diff)) {
      case 1:
	return Poly_Con_Relation::is_disjoint();
      case 0:
	if (i.lower_is_open())
	  return Poly_Con_Relation::is_disjoint();
        if (i.is_singleton())
          return Poly_Con_Relation::is_included()
            && Poly_Con_Relation::saturates();
        return Poly_Con_Relation::strictly_intersects();
      case -1:
	if (i.upper_is_unbounded())
	  return Poly_Con_Relation::strictly_intersects();
	else {
	  assign_r(bound_diff, i.upper(), ROUND_NOT_NEEDED);
	  sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
	  switch (sgn(bound_diff)) {
	  case 1:
	    return Poly_Con_Relation::strictly_intersects();
	  case 0:
	    if (i.upper_is_open())
	      return Poly_Con_Relation::is_disjoint();
	    else
	      return Poly_Con_Relation::strictly_intersects();
	  case -1:
	    return Poly_Con_Relation::is_disjoint();
	  }
	}
      }
    }
  }

  assert(constraint_type != Constraint::EQUALITY);
  if (is_lower_bound) {
    if (i.lower_is_unbounded()) {
      assert(!i.upper_is_unbounded());
      assign_r(bound_diff, i.upper(), ROUND_NOT_NEEDED);
      sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
      switch (sgn(bound_diff)) {
      case 1:
	return Poly_Con_Relation::strictly_intersects();
      case 0:
	if (constraint_type == Constraint::STRICT_INEQUALITY
	    || i.upper_is_open())
	  return Poly_Con_Relation::is_disjoint();
	else
	  return Poly_Con_Relation::strictly_intersects();
      case -1:
	return Poly_Con_Relation::is_disjoint();
      }
    }
    else {
      assign_r(bound_diff, i.lower(), ROUND_NOT_NEEDED);
      sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
      switch (sgn(bound_diff)) {
      case 1:
	return Poly_Con_Relation::is_included();
      case 0:
	if (constraint_type == Constraint::NONSTRICT_INEQUALITY
	    || i.lower_is_open()) {
	  Poly_Con_Relation result = Poly_Con_Relation::is_included();
	  if (i.is_singleton())
	    result = result && Poly_Con_Relation::saturates();
	  return result;
	}
	else {
	  assert(constraint_type == Constraint::STRICT_INEQUALITY
		 && !i.lower_is_open());
	  if (i.is_singleton())
	    return Poly_Con_Relation::is_disjoint()
	      && Poly_Con_Relation::saturates();
	  else
	    return Poly_Con_Relation::strictly_intersects();
	}
      case -1:
	if (i.upper_is_unbounded())
	  return Poly_Con_Relation::strictly_intersects();
	else {
	  assign_r(bound_diff, i.upper(), ROUND_NOT_NEEDED);
	  sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
	  switch (sgn(bound_diff)) {
	  case 1:
	    return Poly_Con_Relation::strictly_intersects();
	  case 0:
	    if (constraint_type == Constraint::STRICT_INEQUALITY
		|| i.upper_is_open())
	      return Poly_Con_Relation::is_disjoint();
	    else
	      return Poly_Con_Relation::strictly_intersects();
	  case -1:
	    return Poly_Con_Relation::is_disjoint();
	  }
	}
      }
    }
  }
  else {
    // `c' is an upper bound.
    if (i.upper_is_unbounded())
      return Poly_Con_Relation::strictly_intersects();
    else {
      assign_r(bound_diff, i.upper(), ROUND_NOT_NEEDED);
      sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
      switch (sgn(bound_diff)) {
      case -1:
	return Poly_Con_Relation::is_included();
      case 0:
	if (constraint_type == Constraint::NONSTRICT_INEQUALITY
	    || i.upper_is_open()) {
	  Poly_Con_Relation result = Poly_Con_Relation::is_included();
	  if (i.is_singleton())
	    result = result && Poly_Con_Relation::saturates();
	  return result;
	}
	else {
	  assert(constraint_type == Constraint::STRICT_INEQUALITY
		 && !i.upper_is_open());
	  if (i.is_singleton())
	    return Poly_Con_Relation::is_disjoint()
	      && Poly_Con_Relation::saturates();
	  else
	    return Poly_Con_Relation::strictly_intersects();
	}
      case 1:
	if (i.lower_is_unbounded())
	  return Poly_Con_Relation::strictly_intersects();
	else {
	  assign_r(bound_diff, i.lower(), ROUND_NOT_NEEDED);
	  sub_assign_r(bound_diff, bound_diff, bound, ROUND_NOT_NEEDED);
	  switch (sgn(bound_diff)) {
	  case -1:
	    return Poly_Con_Relation::strictly_intersects();
	  case 0:
	    if (constraint_type == Constraint::STRICT_INEQUALITY
		|| i.lower_is_open())
	      return Poly_Con_Relation::is_disjoint();
	    else
	      return Poly_Con_Relation::strictly_intersects();
	  case 1:
	    return Poly_Con_Relation::is_disjoint();
	  }
	}
      }
    }
  }

  // Quiet a compiler warning: this program point is unreachable.
  throw std::runtime_error("PPL internal error");
}

template <typename ITV>
Poly_Con_Relation
Box<ITV>::relation_with(const Congruence& cg) const {
  const dimension_type cg_space_dim = cg.space_dimension();
  const dimension_type space_dim = space_dimension();

  // Dimension-compatibility check.
  if (cg_space_dim > space_dim)
    throw_dimension_incompatible("relation_with(cg)", cg);

  if (is_empty())
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

   if (space_dim == 0) {
    if (cg.is_inconsistent())
      return Poly_Con_Relation::is_disjoint();
    else
      return Poly_Con_Relation::saturates()
	&& Poly_Con_Relation::is_included();
  }

  if (cg.is_equality()) {
    const Constraint c(cg);
    return relation_with(c);
  }

  PPL_DIRTY_TEMP0(Rational_Interval, r);
  PPL_DIRTY_TEMP0(Rational_Interval, t);
  PPL_DIRTY_TEMP0(mpq_class, m);
  r = 0;
  for (dimension_type i = cg.space_dimension(); i-- > 0; ) {
    const Coefficient& cg_i = cg.coefficient(Variable(i));
    if (sgn(cg_i) != 0) {
      assign_r(m, cg_i, ROUND_NOT_NEEDED);
      // FIXME: an add_mul_assign() method would come handy here.
      t = seq[i];
      t *= m;
      r += t;
    }
  }

  if (r.lower_is_unbounded() || r.upper_is_unbounded())
    return Poly_Con_Relation::strictly_intersects();


  // Find the value that satisfies the congruence and is
  // nearest to the lower bound such that the point lies on or above it.

  PPL_DIRTY_TEMP_COEFFICIENT(lower);
  PPL_DIRTY_TEMP_COEFFICIENT(mod);
  PPL_DIRTY_TEMP_COEFFICIENT(v);
  mod = cg.modulus();
  v = cg.inhomogeneous_term() % mod;
  assign_r(lower, r.lower(), ROUND_DOWN);
  v -= ((lower / mod) * mod);
  if (v + lower > 0)
    v -= mod;
  return interval_relation(r, Constraint::EQUALITY, v);
}

template <typename ITV>
Poly_Con_Relation
Box<ITV>::relation_with(const Constraint& c) const {
  const dimension_type c_space_dim = c.space_dimension();
  const dimension_type space_dim = space_dimension();

  // Dimension-compatibility check.
  if (c_space_dim > space_dim)
    throw_dimension_incompatible("relation_with(c)", c);

  if (is_empty())
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  if (space_dim == 0) {
    if ((c.is_equality() && c.inhomogeneous_term() != 0)
	|| (c.is_inequality() && c.inhomogeneous_term() < 0))
      return Poly_Con_Relation::is_disjoint();
    else if (c.is_strict_inequality() && c.inhomogeneous_term() == 0)
      // The constraint 0 > 0 implicitly defines the hyperplane 0 = 0;
      // thus, the zero-dimensional point also saturates it.
      return Poly_Con_Relation::saturates()
	&& Poly_Con_Relation::is_disjoint();
    else if (c.is_equality() || c.inhomogeneous_term() == 0)
      return Poly_Con_Relation::saturates()
	&& Poly_Con_Relation::is_included();
    else
      // The zero-dimensional point saturates
      // neither the positivity constraint 1 >= 0,
      // nor the strict positivity constraint 1 > 0.
      return Poly_Con_Relation::is_included();
  }

  dimension_type c_num_vars = 0;
  dimension_type c_only_var = 0;

  if (extract_interval_constraint(c, c_space_dim, c_num_vars, c_only_var))
    if (c_num_vars == 0)
      // c is a trivial constraint.
      switch (sgn(c.inhomogeneous_term())) {
      case -1:
	return Poly_Con_Relation::is_disjoint();
      case 0:
	if (c.is_strict_inequality())
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_disjoint();
	else
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_included();
      case 1:
	return Poly_Con_Relation::is_included();
      }
    else {
      // c is an interval constraint.
      return interval_relation(seq[c_only_var],
			       c.type(),
			       c.inhomogeneous_term(),
			       c.coefficient(Variable(c_only_var)));
    }
  else {
    // Deal with a non-trivial and non-interval constraint.
    PPL_DIRTY_TEMP0(Rational_Interval, r);
    PPL_DIRTY_TEMP0(Rational_Interval, t);
    PPL_DIRTY_TEMP0(mpq_class, m);
    r = 0;
    for (dimension_type i = c.space_dimension(); i-- > 0; ) {
      const Coefficient& c_i = c.coefficient(Variable(i));
      if (sgn(c_i) != 0) {
        assign_r(m, c_i, ROUND_NOT_NEEDED);
	// FIXME: an add_mul_assign() method would come handy here.
	t = seq[i];
	t *= m;
	r += t;
      }
    }
    return interval_relation(r,
			     c.type(),
			     c.inhomogeneous_term());
  }

  // Quiet a compiler warning: this program point is unreachable.
  throw std::runtime_error("PPL internal error");
}

template <typename ITV>
Poly_Gen_Relation
Box<ITV>::relation_with(const Generator& g) const {
  const dimension_type space_dim = space_dimension();
  const dimension_type g_space_dim = g.space_dimension();

  // Dimension-compatibility check.
  if (space_dim < g_space_dim)
    throw_dimension_incompatible("relation_with(g)", g);

  // The empty box cannot subsume a generator.
  if (is_empty())
    return Poly_Gen_Relation::nothing();

  // A universe box in a zero-dimensional space subsumes
  // all the generators of a zero-dimensional space.
  if (space_dim == 0)
    return Poly_Gen_Relation::subsumes();

  if (g.is_line_or_ray()) {
    if (g.is_line()) {
      for (dimension_type i = g_space_dim; i-- > 0; )
	if (g.coefficient(Variable(i)) != 0 && !seq[i].is_universe())
	  return Poly_Gen_Relation::nothing();
      return Poly_Gen_Relation::subsumes();
    }
    else {
      assert(g.is_ray());
      for (dimension_type i = g_space_dim; i-- > 0; )
	switch (sgn(g.coefficient(Variable(i)))) {
	case 1:
	  if (!seq[i].upper_is_unbounded())
	    return Poly_Gen_Relation::nothing();
	  break;
	case 0:
	  break;
	case -1:
	  if (!seq[i].lower_is_unbounded())
	    return Poly_Gen_Relation::nothing();
	  break;
	}
      return Poly_Gen_Relation::subsumes();
    }
  }

  // Here `g' is a point or closure point.
  const Coefficient& g_divisor = g.divisor();
  PPL_DIRTY_TEMP0(mpq_class, g_coord);
  PPL_DIRTY_TEMP0(mpq_class, bound);
  for (dimension_type i = g_space_dim; i-- > 0; ) {
    const ITV& seq_i = seq[i];
    if (seq_i.is_universe())
      continue;
    assign_r(g_coord.get_num(), g.coefficient(Variable(i)), ROUND_NOT_NEEDED);
    assign_r(g_coord.get_den(), g_divisor, ROUND_NOT_NEEDED);
    g_coord.canonicalize();
    // Check lower bound.
    if (!seq_i.lower_is_unbounded()) {
      assign_r(bound, seq_i.lower(), ROUND_NOT_NEEDED);
      if (g_coord <= bound) {
	if (seq_i.lower_is_open()) {
	  if (g.is_point() || g_coord != bound)
	    return Poly_Gen_Relation::nothing();
	}
	else if (g_coord != bound)
	  return Poly_Gen_Relation::nothing();
      }
    }
    // Check upper bound.
    if (!seq_i.upper_is_unbounded()) {
      assign_r(bound, seq_i.upper(), ROUND_NOT_NEEDED);
      if (g_coord >= bound) {
	if (seq_i.upper_is_open()) {
	  if (g.is_point() || g_coord != bound)
	    return Poly_Gen_Relation::nothing();
	}
	else if (g_coord != bound)
	  return Poly_Gen_Relation::nothing();
      }
    }
  }
  return Poly_Gen_Relation::subsumes();
}


template <typename ITV>
bool
Box<ITV>::max_min(const Linear_Expression& expr,
                  const bool maximize,
                  Coefficient& ext_n, Coefficient& ext_d,
                  bool& included) const {
  // `expr' should be dimension-compatible with `*this'.
  const dimension_type space_dim = space_dimension();
  const dimension_type expr_space_dim = expr.space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible((maximize
				  ? "maximize(e, ...)"
				  : "minimize(e, ...)"), "e", expr);
  // Deal with zero-dim Box first.
  if (space_dim == 0) {
    if (marked_empty())
      return false;
    else {
      ext_n = expr.inhomogeneous_term();
      ext_d = 1;
      included = true;
      return true;
    }
  }

  // For an empty Box we simply return false.
  if (is_empty())
    return false;

  PPL_DIRTY_TEMP0(mpq_class, result);
  assign_r(result, expr.inhomogeneous_term(), ROUND_NOT_NEEDED);
  bool is_included = true;
  const int maximize_sign = maximize ? 1 : -1;
  PPL_DIRTY_TEMP0(mpq_class, bound_i);
  PPL_DIRTY_TEMP0(mpq_class, expr_i);
  for (dimension_type i = expr_space_dim; i-- > 0; ) {
    const ITV& seq_i = seq[i];
    assign_r(expr_i, expr.coefficient(Variable(i)), ROUND_NOT_NEEDED);
    switch (sgn(expr_i) * maximize_sign) {
    case 1:
      if (seq_i.upper_is_unbounded())
	return false;
      assign_r(bound_i, seq_i.upper(), ROUND_NOT_NEEDED);
      add_mul_assign_r(result, bound_i, expr_i, ROUND_NOT_NEEDED);
      if (seq_i.upper_is_open())
	is_included = false;
      break;
    case 0:
      // Nothing to do.
      break;
    case -1:
      if (seq_i.lower_is_unbounded())
	return false;
      assign_r(bound_i, seq_i.lower(), ROUND_NOT_NEEDED);
      add_mul_assign_r(result, bound_i, expr_i, ROUND_NOT_NEEDED);
      if (seq_i.lower_is_open())
	is_included = false;
      break;
    }
  }
  // Extract output info.
  assert(is_canonical(result));
  ext_n = result.get_num();
  ext_d = result.get_den();
  included = is_included;
  return true;
}

template <typename ITV>
bool
Box<ITV>::max_min(const Linear_Expression& expr,
                  const bool maximize,
                  Coefficient& ext_n, Coefficient& ext_d,
                  bool& included,
                  Generator& g) const {
  if (!max_min(expr, maximize, ext_n, ext_d, included))
    return false;

  // Compute generator `g'.
  Linear_Expression g_expr;
  PPL_DIRTY_TEMP(Coefficient, g_divisor);
  g_divisor = 1;
  const int maximize_sign = maximize ? 1 : -1;
  PPL_DIRTY_TEMP0(mpq_class, g_coord);
  PPL_DIRTY_TEMP(Coefficient, num);
  PPL_DIRTY_TEMP(Coefficient, den);
  PPL_DIRTY_TEMP(Coefficient, lcm);
  PPL_DIRTY_TEMP(Coefficient, factor);
  for (dimension_type i = space_dimension(); i-- > 0; ) {
    const ITV& seq_i = seq[i];
    switch (sgn(expr.coefficient(Variable(i))) * maximize_sign) {
    case 1:
      assign_r(g_coord, seq_i.upper(), ROUND_NOT_NEEDED);
      break;
    case 0:
      // If 0 belongs to the interval, choose it
      // (and directly proceed to the next iteration).
      // FIXME: name qualification issue.
      if (seq_i.contains(0))
	continue;
      if (!seq_i.lower_is_unbounded())
	if (seq_i.lower_is_open())
	  if (!seq_i.upper_is_unbounded())
	    if (seq_i.upper_is_open()) {
	      // Bounded and open interval: compute middle point.
	      assign_r(g_coord, seq_i.lower(), ROUND_NOT_NEEDED);
	      PPL_DIRTY_TEMP0(mpq_class, q_seq_i_upper);
	      assign_r(q_seq_i_upper, seq_i.upper(), ROUND_NOT_NEEDED);
	      g_coord += q_seq_i_upper;
	      g_coord /= 2;
	    }
	    else
	      // The upper bound is in the interval.
	      assign_r(g_coord, seq_i.upper(), ROUND_NOT_NEEDED);
	  else {
	    // Lower is open, upper is unbounded.
	    assign_r(g_coord, seq_i.lower(), ROUND_NOT_NEEDED);
	    ++g_coord;
	  }
	else
	  // The lower bound is in the interval.
	  assign_r(g_coord, seq_i.lower(), ROUND_NOT_NEEDED);
      else {
	// Lower is unbounded, hence upper is bounded
	// (since we know that 0 does not belong to the interval).
	assert(!seq_i.upper_is_unbounded());
	assign_r(g_coord, seq_i.upper(), ROUND_NOT_NEEDED);
	if (seq_i.upper_is_open())
	  --g_coord;
      }
      break;
    case -1:
      assign_r(g_coord, seq_i.lower(), ROUND_NOT_NEEDED);
      break;
    }
    // Add g_coord * Variable(i) to the generator.
    assign_r(den, g_coord.get_den(), ROUND_NOT_NEEDED);
    lcm_assign(lcm, g_divisor, den);
    exact_div_assign(factor, lcm, g_divisor);
    g_expr *= factor;
    exact_div_assign(factor, lcm, den);
    assign_r(num, g_coord.get_num(), ROUND_NOT_NEEDED);
    num *= factor;
    g_expr += num * Variable(i);
    g_divisor = lcm;
  }
  g = Generator::point(g_expr, g_divisor);
  return true;
}

template <typename ITV>
bool
Box<ITV>::contains(const Box& y) const {
  const Box& x = *this;
  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    x.throw_dimension_incompatible("contains(y)", y);

  // If `y' is empty, then `x' contains `y'.
  if (y.is_empty())
    return true;

  // If `x' is empty, then `x' cannot contain `y'.
  if (x.is_empty())
    return false;

  for (dimension_type k = x.seq.size(); k-- > 0; )
    // FIXME: fix this name qualification issue.
    if (!x.seq[k].contains(y.seq[k]))
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::is_disjoint_from(const Box& y) const {
  const Box& x = *this;
  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    x.throw_dimension_incompatible("is_disjoint_from(y)", y);

  // If any of `x' or `y' is marked empty, then they are disjoint.
  // Note: no need to use `is_empty', as the following loop is anyway correct.
  if (x.marked_empty() || y.marked_empty())
    return true;

  for (dimension_type k = x.seq.size(); k-- > 0; )
    // FIXME: fix this name qualification issue.
    if (x.seq[k].is_disjoint_from(y.seq[k]))
      return true;
  return false;
}

template <typename ITV>
inline bool
Box<ITV>::upper_bound_assign_if_exact(const Box& y) {
  Box& x = *this;

  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    x.throw_dimension_incompatible("upper_bound_assign_if_exact(y)", y);

  // The lub of a box with an empty box is equal to the first box.
  if (y.marked_empty())
    return true;
  if (x.marked_empty()) {
    x = y;
    return true;
  }

  bool x_j_does_not_contain_y_j = false;
  bool y_j_does_not_contain_x_j = false;

  for (dimension_type i = x.seq.size(); i-- > 0; ) {
    const ITV& x_seq_i = x.seq[i];
    const ITV& y_seq_i = y.seq[i];

    if (!x_seq_i.can_be_exactly_joined_to(y_seq_i))
      return false;

    // Note: the use of `y_i_does_not_contain_x_i' is needed
    // because we want to temporarily preserve the old value
    // of `y_j_does_not_contain_x_j'.
    bool y_i_does_not_contain_x_i = !y_seq_i.contains(x_seq_i);
    if (y_i_does_not_contain_x_i && x_j_does_not_contain_y_j)
      return false;
    if (!x_seq_i.contains(y_seq_i)) {
      if (y_j_does_not_contain_x_j)
        return false;
      else
        x_j_does_not_contain_y_j = true;
    }
    if (y_i_does_not_contain_x_i)
      y_j_does_not_contain_x_j = true;
  }

  // The upper bound is exact: compute it into *this.
  for (dimension_type k = x.seq.size(); k-- > 0; )
    x.seq[k].join_assign(y.seq[k]);
  return true;
}

template <typename ITV>
bool
Box<ITV>::OK() const {
  if (status.test_empty_up_to_date() && !status.test_empty()) {
    Box tmp = *this;
    tmp.reset_empty_up_to_date();
    if (tmp.check_empty()) {
#ifndef NDEBUG
      std::cerr << "The box is empty, but it is marked as non-empty."
		<< std::endl;
#endif // NDEBUG
      return false;
    }
  }

  // A box that is not marked empty must have meaningful intervals.
  if (!marked_empty()) {
    for (dimension_type k = seq.size(); k-- > 0; )
      if (!seq[k].OK())
	return false;
  }

  return true;
}

template <typename ITV>
dimension_type
Box<ITV>::affine_dimension() const {
  dimension_type d = space_dimension();
  // A zero-space-dim box always has affine dimension zero.
  if (d == 0)
    return 0;

  // An empty box has affine dimension zero.
  if (is_empty())
    return 0;

  for (dimension_type k = d; k-- > 0; )
    if (seq[k].is_singleton())
      --d;

  return d;
}

template <typename ITV>
bool
Box<ITV>::check_empty() const {
  assert(!marked_empty());
  Box<ITV>& x = const_cast<Box<ITV>&>(*this);
  for (dimension_type k = seq.size(); k-- > 0; )
    if (seq[k].is_empty()) {
      x.set_empty();
      return true;
    }
  x.set_nonempty();;
  return false;
}

template <typename ITV>
bool
Box<ITV>::is_universe() const {
  if (marked_empty())
    return false;
  for (dimension_type k = seq.size(); k-- > 0; )
    if (!seq[k].is_universe())
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::is_topologically_closed() const {
  if (!ITV::info_type::store_open || is_empty())
    return true;

  for (dimension_type k = seq.size(); k-- > 0; )
    if (!seq[k].is_topologically_closed())
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::is_discrete() const {
  if (is_empty())
    return true;
  for (dimension_type k = seq.size(); k-- > 0; )
    if (!seq[k].is_singleton())
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::is_bounded() const {
  if (is_empty())
    return true;
  for (dimension_type k = seq.size(); k-- > 0; )
    if (seq[k].is_unbounded())
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::contains_integer_point() const {
  if (marked_empty())
    return false;
  for (dimension_type k = seq.size(); k-- > 0; )
    if (!seq[k].contains_integer_point())
      return false;
  return true;
}

template <typename ITV>
bool
Box<ITV>::constrains(Variable var) const {
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dimension() < var_space_dim)
    throw_dimension_incompatible("constrains(v)", "v", var);

  if (marked_empty() || !seq[var_space_dim-1].is_universe())
    return true;
  // Now force an emptiness check.
  return is_empty();
}

template <typename ITV>
void
Box<ITV>::unconstrain(const Variables_Set& to_be_unconstrained) {
  // The cylindrification wrt no dimensions is a no-op.
  // This case also captures the only legal cylindrification
  // of a box in a 0-dim space.
  if (to_be_unconstrained.empty())
    return;

  // Dimension-compatibility check.
  const dimension_type min_space_dim = to_be_unconstrained.space_dimension();
  if (space_dimension() < min_space_dim)
    throw_dimension_incompatible("unconstrain(vs)", min_space_dim);

  // If the box is already empty, there is nothing left to do.
  if (marked_empty())
    return;

  // Here the box might still be empty (but we haven't detected it yet):
  // check emptiness of the interval for each of the variables in
  // `to_be_unconstrained' before cylindrification.
  for (Variables_Set::const_iterator tbu = to_be_unconstrained.begin(),
         tbu_end = to_be_unconstrained.end(); tbu != tbu_end; ++tbu) {
    ITV& seq_tbu = seq[*tbu];
    if (!seq_tbu.is_empty())
      seq_tbu.assign(UNIVERSE);
    else {
      set_empty();
      break;
    }
  }
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::topological_closure_assign() {
  if (!ITV::info_type::store_open || is_empty())
    return;

  for (dimension_type k = seq.size(); k-- > 0; )
    seq[k].topological_closure_assign();
}

template <typename ITV>
void
Box<ITV>::intersection_assign(const Box& y) {
  Box& x = *this;
  const dimension_type space_dim = space_dimension();

  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    x.throw_dimension_incompatible("intersection_assign(y)", y);

  // If one of the two boxes is empty, the intersection is empty.
  if (x.marked_empty())
    return;
  if (y.marked_empty()) {
    x.set_empty();
    return;
  }

  // If both boxes are zero-dimensional, then at this point they are
  // necessarily non-empty, so that their intersection is non-empty too.
  if (space_dim == 0)
    return;

  // FIXME: here we may conditionally exploit a capability of the
  // underlying interval to eagerly detect empty results.
  reset_empty_up_to_date();

  for (dimension_type k = space_dim; k-- > 0; )
    x.seq[k].intersect_assign(y.seq[k]);

  assert(x.OK());
}

template <typename ITV>
void
Box<ITV>::upper_bound_assign(const Box& y) {
  Box& x = *this;

  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    x.throw_dimension_incompatible("upper_bound_assign(y)", y);

  // The lub of a box with an empty box is equal to the first box.
  if (y.marked_empty())
    return;
  if (x.marked_empty()) {
    x = y;
    return;
  }

  for (dimension_type k = x.seq.size(); k-- > 0; )
    x.seq[k].join_assign(y.seq[k]);

  assert(x.OK());
}

template <typename ITV>
void
Box<ITV>::concatenate_assign(const Box& y) {
  Box& x = *this;
  const dimension_type x_space_dim = x.space_dimension();
  const dimension_type y_space_dim = y.space_dimension();

  // If `y' is marked empty, the result will be empty too.
  if (y.marked_empty())
    x.set_empty();

  // If `y' is a 0-dim space box, there is nothing left to do.
  if (y_space_dim == 0)
    return;

  // Here `y_space_dim > 0', so that a non-trivial concatenation will occur:
  // make sure that reallocation will occur once at most.
  x.seq.reserve(x_space_dim + y_space_dim);

  // If `x' is marked empty, then it is sufficient to adjust
  // the dimension of the vector space.
  if (x.marked_empty()) {
    x.seq.insert(x.seq.end(), y_space_dim, ITV(EMPTY));
    assert(x.OK());
    return;
  }

  // Here neither `x' nor `y' are marked empty: concatenate them.
  std::copy(y.seq.begin(), y.seq.end(),
	    std::back_insert_iterator<Sequence>(x.seq));
  // Update the `empty_up_to_date' flag.
  if (!y.status.test_empty_up_to_date())
    reset_empty_up_to_date();

  assert(x.OK());
}

template <typename ITV>
void
Box<ITV>::difference_assign(const Box& y) {
  const dimension_type space_dim = space_dimension();

  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    throw_dimension_incompatible("difference_assign(y)", y);

  Box& x = *this;
  if (x.is_empty() || y.is_empty())
    return;

  switch (space_dim) {
  case 0:
    // If `x' is zero-dimensional, then at this point both `x' and `y'
    // are the universe box, so that their difference is empty.
    x.set_empty();
    break;

  case 1:
    x.seq[0].difference_assign(y.seq[0]);
    if (x.seq[0].is_empty())
      x.set_empty();
    break;

  default:
    {
      dimension_type index_non_contained = space_dim;
      dimension_type number_non_contained = 0;
      for (dimension_type i = space_dim; i-- > 0; )
        if (!y.seq[i].contains(x.seq[i])) {
          if (++number_non_contained == 1)
            index_non_contained = i;
          else
            break;
        }

      switch (number_non_contained) {
      case 0:
        // `y' covers `x': the difference is empty.
        x.set_empty();
        break;
      case 1:
        x.seq[index_non_contained]
          .difference_assign(y.seq[index_non_contained]);
        if (x.seq[index_non_contained].is_empty())
          x.set_empty();
        break;
      default:
        // Nothing to do: the difference is `x'.
        break;
      }
    }
    break;
  }
  assert(OK());
}

template <typename ITV>
bool
Box<ITV>::simplify_using_context_assign(const Box& y) {
  Box& x = *this;
  const dimension_type num_dims = x.space_dimension();
  // Dimension-compatibility check.
  if (num_dims != y.space_dimension())
    x.throw_dimension_incompatible("simplify_using_context_assign(y)", y);

  // Filter away the zero-dimensional case.
  if (num_dims == 0) {
    if (y.marked_empty()) {
      x.set_nonempty();
      return false;
    }
    else
      return !x.marked_empty();
  }

  // Filter away the case when `y' is empty.
  if (y.is_empty()) {
    for (dimension_type i = num_dims; i-- > 0; )
      x.seq[i].assign(UNIVERSE);
    x.set_nonempty();
    return false;
  }

  if (x.is_empty()) {
    // Find in `y' a non-universe interval, if any.
    for (dimension_type i = 0; i < num_dims; ++i) {
      if (y.seq[i].is_universe())
        x.seq[i].assign(UNIVERSE);
      else {
        // Set x.seq[i] so as to contradict y.seq[i], if possible.
        ITV& seq_i = x.seq[i];
        seq_i.empty_intersection_assign(y.seq[i]);
        if (seq_i.is_empty()) {
          // We were not able to assign to `seq_i' a non-empty interval:
          // reset `seq_i' to the universe interval and keep searching.
          seq_i.assign(UNIVERSE);
          continue;
        }
        // We assigned to `seq_i' a non-empty interval:
        // set the other intervals to universe and return.
        for (++i; i < num_dims; ++i)
          x.seq[i].assign(UNIVERSE);
        x.set_nonempty();
        assert(x.OK());
        return false;
      }
    }
    // All intervals in `y' are universe or could not be contradicted:
    // simplification can leave the empty box `x' as is.
    assert(x.OK() && x.is_empty());
    return false;
  }

  // Loop index `i' is intentionally going upwards.
  dimension_type i = 0;
  for ( ; i < num_dims; ++i) {
    if (!x.seq[i].simplify_using_context_assign(y.seq[i])) {
      assert(!x.seq[i].is_empty());
      // The intersection of `x' and `y' is empty due to the i-th interval:
      // reset other intervals to UNIVERSE.
      for (dimension_type j = num_dims; j-- > i; )
        x.seq[j].assign(UNIVERSE);
      for (dimension_type j = i; j-- > 0; )
        x.seq[j].assign(UNIVERSE);
      assert(x.OK());
      return false;
    }
  }
  assert(x.OK());
  return true;
}

template <typename ITV>
void
Box<ITV>::time_elapse_assign(const Box& y) {
  Box& x = *this;
  const dimension_type x_space_dim = x.space_dimension();

  // Dimension-compatibility check.
  if (x_space_dim != y.space_dimension())
    x.throw_dimension_incompatible("time_elapse_assign(y)", y);

  // Dealing with the zero-dimensional case.
  if (x_space_dim == 0) {
    if (y.marked_empty())
      x.set_empty();
    return;
  }

  // If either one of `x' or `y' is empty, the result is empty too.
  // Note: if possible, avoid cost of checking for emptiness.
  if (x.marked_empty() || y.marked_empty()
      || x.is_empty() || y.is_empty()) {
    x.set_empty();
    return;
  }

  for (dimension_type i = x_space_dim; i-- > 0; ) {
    ITV& x_seq_i = x.seq[i];
    const ITV& y_seq_i = y.seq[i];
    if (!x_seq_i.lower_is_unbounded())
      if (y_seq_i.lower_is_unbounded() || y_seq_i.lower() < 0)
	x_seq_i.lower_set(UNBOUNDED);
    if (!x_seq_i.upper_is_unbounded())
      if (y_seq_i.upper_is_unbounded() || y_seq_i.upper() > 0)
	x_seq_i.upper_set(UNBOUNDED);
  }
  assert(x.OK());
}

template <typename ITV>
inline void
Box<ITV>::remove_space_dimensions(const Variables_Set& to_be_removed) {
  // The removal of no dimensions from any box is a no-op.
  // Note that this case also captures the only legal removal of
  // space dimensions from a box in a zero-dimensional space.
  if (to_be_removed.empty()) {
    assert(OK());
    return;
  }

  const dimension_type old_space_dim = space_dimension();

  // Dimension-compatibility check.
  const dimension_type tbr_space_dim = to_be_removed.space_dimension();
  if (old_space_dim < tbr_space_dim)
    throw_dimension_incompatible("remove_space_dimensions(vs)",
				 tbr_space_dim);

  const dimension_type new_space_dim = old_space_dim - to_be_removed.size();

  // If the box is empty (this must be detected), then resizing is all
  // what is needed.  If it is not empty and we are removing _all_ the
  // dimensions then, again, resizing suffices.
  if (is_empty() || new_space_dim == 0) {
    seq.resize(new_space_dim);
    assert(OK());
    return;
  }

  // For each variable to be removed, we fill the corresponding interval
  // by shifting left those intervals that will not be removed.
  Variables_Set::const_iterator tbr = to_be_removed.begin();
  Variables_Set::const_iterator tbr_end = to_be_removed.end();
  dimension_type dst = *tbr;
  dimension_type src = dst + 1;
  for (++tbr; tbr != tbr_end; ++tbr) {
    const dimension_type tbr_next = *tbr;
    // All intervals in between are moved to the left.
    while (src < tbr_next)
      seq[dst++].swap(seq[src++]);
    ++src;
  }
  // Moving the remaining intervals.
  while (src < old_space_dim)
    seq[dst++].swap(seq[src++]);

  assert(dst == new_space_dim);
  seq.resize(new_space_dim);

  assert(OK());
}

template <typename ITV>
void
Box<ITV>::remove_higher_space_dimensions(const dimension_type new_dim) {
  // Dimension-compatibility check: the variable having
  // maximum index is the one occurring last in the set.
  const dimension_type old_dim = space_dimension();
  if (new_dim > old_dim)
    throw_dimension_incompatible("remove_higher_space_dimensions(nd)",
				 new_dim);

  // The removal of no dimensions from any box is a no-op.
  // Note that this case also captures the only legal removal of
  // dimensions from a zero-dim space box.
  if (new_dim == old_dim) {
    assert(OK());
    return;
  }

  seq.erase(seq.begin() + new_dim, seq.end());
  assert(OK());
}

template <typename ITV>
template <typename Partial_Function>
void
Box<ITV>::map_space_dimensions(const Partial_Function& pfunc) {
  const dimension_type space_dim = space_dimension();
  if (space_dim == 0)
    return;

  if (pfunc.has_empty_codomain()) {
    // All dimensions vanish: the box becomes zero_dimensional.
    remove_higher_space_dimensions(0);
    return;
  }

  const dimension_type new_space_dim = pfunc.max_in_codomain() + 1;
  // If the box is empty, then simply adjust the space dimension.
  if (is_empty()) {
    remove_higher_space_dimensions(new_space_dim);
    return;
  }

  // We create a new Box with the new space dimension.
  Box<ITV> tmp(new_space_dim);
  // Map the intervals, exchanging the indexes.
  for (dimension_type i = 0; i < space_dim; ++i) {
    dimension_type new_i;
    if (pfunc.maps(i, new_i))
      seq[i].swap(tmp.seq[new_i]);
  }
  swap(tmp);
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::fold_space_dimensions(const Variables_Set& to_be_folded,
                                const Variable var) {
  const dimension_type space_dim = space_dimension();
  // `var' should be one of the dimensions of the box.
  if (var.space_dimension() > space_dim)
    throw_dimension_incompatible("fold_space_dimensions(tbf, v)", "v", var);

  // The folding of no dimensions is a no-op.
  if (to_be_folded.empty())
    return;

  // All variables in `to_be_folded' should be dimensions of the box.
  if (to_be_folded.space_dimension() > space_dim)
    throw_dimension_incompatible("fold_space_dimensions(tbf, ...)",
				 to_be_folded.space_dimension());

  // Moreover, `var.id()' should not occur in `to_be_folded'.
  if (to_be_folded.find(var.id()) != to_be_folded.end())
    throw_generic("fold_space_dimensions(tbf, v)",
		  "v should not occur in tbf");

  // Note: the check for emptiness is needed for correctness.
  if (!is_empty()) {
    // Join the interval corresponding to variable `var' with the intervals
    // corresponding to the variables in `to_be_folded'.
    ITV& seq_v = seq[var.id()];
    for (Variables_Set::const_iterator i = to_be_folded.begin(),
	   tbf_end = to_be_folded.end(); i != tbf_end; ++i)
      seq_v.join_assign(seq[*i]);
  }
  remove_space_dimensions(to_be_folded);
}

template <typename ITV>
void
Box<ITV>::add_constraint_no_check(const Constraint& c) {
  const dimension_type c_space_dim = c.space_dimension();
  assert(c_space_dim <= space_dimension());

  dimension_type c_num_vars = 0;
  dimension_type c_only_var = 0;
  // Throw an exception if c is not an interval constraints.
  if (!extract_interval_constraint(c, c_space_dim, c_num_vars, c_only_var))
    throw_generic("add_constraint(c)", "c is not an interval constraint");

  // Throw an exception if c is a nontrivial strict constraint
  // and ITV does not support open boundaries.
  if (c.is_strict_inequality() && c_num_vars != 0
      && !Box::interval_type::info_type::store_open)
    throw_generic("add_constraint(c)", "c is a nontrivial strict constraint");

  // Avoid doing useless work if the box is known to be empty.
  if (marked_empty())
    return;

  const Coefficient& n = c.inhomogeneous_term();
  if (c_num_vars == 0) {
    // Dealing with a trivial constraint.
    if (n < 0
        || (c.is_equality() && n != 0)
	|| (c.is_strict_inequality() && n == 0))
      set_empty();
    return;
  }

  assert(c_num_vars == 1);
  const Coefficient& d = c.coefficient(Variable(c_only_var));
  add_interval_constraint_no_check(c_only_var, c.type(), n, d);
}

template <typename ITV>
void
Box<ITV>::add_constraints_no_check(const Constraint_System& cs) {
  assert(cs.space_dimension() <= space_dimension());
  // Note: even when the box is known to be empty, we need to go
  // through all the constraints to fulfill the method's contract
  // for what concerns exception throwing.
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); i != cs_end; ++i)
    add_constraint_no_check(*i);
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::add_congruence_no_check(const Congruence& cg) {
  const dimension_type cg_space_dim = cg.space_dimension();
  assert(cg_space_dim <= space_dimension());

  // Set aside the case of proper congruences.
  if (cg.is_proper_congruence()) {
    if (cg.is_inconsistent()) {
      set_empty();
      return;
    }
    else if (cg.is_tautological())
      return;
    else
      // FIXME: what about intervals with restrictions?
      throw_generic("add_congruence(cg)",
                    "cg is a nontrivial proper congruence");
  }

  assert(cg.is_equality());
  dimension_type cg_num_vars = 0;
  dimension_type cg_only_var = 0;
  // Throw an exception if c is not an interval congruence.
  if (!extract_interval_congruence(cg, cg_space_dim, cg_num_vars, cg_only_var))
    throw_generic("add_congruence(cg)", "cg is not an interval congruence");

  // Avoid doing useless work if the box is known to be empty.
  if (marked_empty())
    return;

  const Coefficient& n = cg.inhomogeneous_term();
  if (cg_num_vars == 0) {
    // Dealing with a trivial equality congruence.
    if (n != 0)
      set_empty();
    return;
  }

  assert(cg_num_vars == 1);
  const Coefficient& d = cg.coefficient(Variable(cg_only_var));
  add_interval_constraint_no_check(cg_only_var, Constraint::EQUALITY, n, d);
}

template <typename ITV>
void
Box<ITV>::add_congruences_no_check(const Congruence_System& cgs) {
  assert(cgs.space_dimension() <= space_dimension());
  // Note: even when the box is known to be empty, we need to go
  // through all the congruences to fulfill the method's contract
  // for what concerns exception throwing.
  for (Congruence_System::const_iterator i = cgs.begin(),
	 cgs_end = cgs.end(); i != cgs_end; ++i)
    add_congruence_no_check(*i);
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::refine_no_check(const Constraint& c) {
  const dimension_type c_space_dim = c.space_dimension();
  assert(c_space_dim <= space_dimension());
  assert(!marked_empty());

  dimension_type c_num_vars = 0;
  dimension_type c_only_var = 0;
  // Non-interval constraints are ignored.
  // FIXME: instead of ignoring, safely use propagate_no_check()
  // (i.e., ensuring that no termination problem can arise).
  if (!extract_interval_constraint(c, c_space_dim, c_num_vars, c_only_var))
    return;

  const Coefficient& n = c.inhomogeneous_term();
  if (c_num_vars == 0) {
    // Dealing with a trivial constraint.
    if (n < 0
        || (c.is_equality() && n != 0)
        || (c.is_strict_inequality() && n == 0))
      set_empty();
    return;
  }

  assert(c_num_vars == 1);
  const Coefficient& d = c.coefficient(Variable(c_only_var));
  add_interval_constraint_no_check(c_only_var, c.type(), n, d);
}

template <typename ITV>
void
Box<ITV>::refine_no_check(const Constraint_System& cs) {
  assert(cs.space_dimension() <= space_dimension());
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); !marked_empty() && i != cs_end; ++i)
    refine_no_check(*i);
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::refine_no_check(const Congruence& cg) {
  assert(!marked_empty());

  const dimension_type cg_space_dim = cg.space_dimension();
  assert(cg_space_dim <= space_dimension());

  if (cg.is_proper_congruence()) {
    // FIXME: also deal with the case of interval with restrictions.
    // A proper congruences is also an interval constraint
    // if and only if it is trivial.
    if (cg.is_inconsistent())
      set_empty();
    return;
  }

  assert(cg.is_equality());
  dimension_type cg_num_vars = 0;
  dimension_type cg_only_var = 0;
  // Congruences that are not interval congruences are ignored.
  if (!extract_interval_congruence(cg, cg_space_dim, cg_num_vars, cg_only_var))
    return;

  if (cg_num_vars == 0) {
    // Dealing with a trivial congruence.
    if (cg.inhomogeneous_term() != 0)
      set_empty();
    return;
  }

  assert(cg_num_vars == 1);
  const Coefficient& n = cg.inhomogeneous_term();
  const Coefficient& d = cg.coefficient(Variable(cg_only_var));
  add_interval_constraint_no_check(cg_only_var, Constraint::EQUALITY, n, d);
}

template <typename ITV>
void
Box<ITV>::refine_no_check(const Congruence_System& cgs) {
  assert(cgs.space_dimension() <= space_dimension());
  for (Congruence_System::const_iterator i = cgs.begin(),
	 cgs_end = cgs.end(); !marked_empty() && i != cgs_end; ++i)
    refine_no_check(*i);
  assert(OK());
}

#if 1 // Alternative implementations for propagate_constraint_no_check.
namespace {

inline bool
propagate_constraint_check_result(Result r, Ternary& open) {
  switch (r) {
  case V_NEG_OVERFLOW:
  case V_POS_OVERFLOW:
  case V_UNKNOWN_NEG_OVERFLOW:
  case V_UNKNOWN_POS_OVERFLOW:
    return true;
  case V_LT:
  case V_GT:
    open = T_YES;
    return false;
  case V_LE:
  case V_GE:
    if (open == T_NO)
      open = T_MAYBE;
    return false;
  case V_EQ:
    return false;
  default:
    assert(false);
    return true;
  }
}

} // namespace

template <typename ITV>
void
Box<ITV>::propagate_constraint_no_check(const Constraint& c) {
  assert(c.space_dimension() <= space_dimension());

  typedef
    typename Select_Temp_Boundary_Type<typename ITV::boundary_type>::type
    Temp_Boundary_Type;

  const dimension_type c_space_dim = c.space_dimension();
  const Constraint::Type c_type = c.type();
  const Coefficient& c_inhomogeneous_term = c.inhomogeneous_term();

  // Find a space dimension having a non-zero coefficient (if any).
  dimension_type last_k = c_space_dim;
  for (dimension_type k = c_space_dim; k-- > 0; ) {
    if (c.coefficient(Variable(k)) != 0) {
      last_k = k;
      break;
    }
  }
  if (last_k == c_space_dim) {
    // Constraint c is trivial: check if it is inconsistent.
    if (c_inhomogeneous_term < 0
        || (c_inhomogeneous_term == 0
            && c_type != Constraint::NONSTRICT_INEQUALITY))
      set_empty();
    return;
  }

  // Here constraint c is non-trivial.
  assert(last_k < c_space_dim);
  Result r;
  Temp_Boundary_Type t_bound;
  Temp_Boundary_Type t_a;
  Temp_Boundary_Type t_x;
  Ternary open;
  for (dimension_type k = last_k+1; k-- > 0; ) {
    const Coefficient& a_k = c.coefficient(Variable(k));
    int sgn_a_k = sgn(a_k);
    if (sgn_a_k == 0)
      continue;
    if (sgn_a_k > 0) {
      open = (c_type == Constraint::STRICT_INEQUALITY) ? T_YES : T_NO;
      if (open == T_NO)
	maybe_reset_fpu_inexact<Temp_Boundary_Type>();
      r = assign_r(t_bound, c_inhomogeneous_term, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_1;
      r = neg_assign_r(t_bound, t_bound, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_1;
      for (dimension_type i = last_k+1; i-- > 0; ) {
	if (i == k)
	  continue;
	const Coefficient& a_i = c.coefficient(Variable(i));
	int sgn_a_i = sgn(a_i);
	if (sgn_a_i == 0)
	  continue;
	ITV& x_i = seq[i];
	if (sgn_a_i < 0) {
	  if (x_i.lower_is_unbounded())
	    goto maybe_refine_upper_1;
	  r = assign_r(t_a, a_i, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	  r = assign_r(t_x, x_i.lower(), ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	  if (x_i.lower_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	}
	else {
	  assert(sgn_a_i > 0);
	  if (x_i.upper_is_unbounded())
	    goto maybe_refine_upper_1;
	  r = assign_r(t_a, a_i, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	  r = assign_r(t_x, x_i.upper(), ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	  if (x_i.upper_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_1;
	}
      }
      r = assign_r(t_a, a_k, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_1;
      r = div_assign_r(t_bound, t_bound, t_a, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_1;

      // Refine the lower bound of `seq[k]' with `t_bound'.
      if (open == T_MAYBE
	  && maybe_check_fpu_inexact<Temp_Boundary_Type>() == 1)
	open = T_YES;
      seq[k].lower_narrow(t_bound, open == T_YES);
      reset_empty_up_to_date();
    maybe_refine_upper_1:
      if (c_type != Constraint::EQUALITY)
	continue;
      open = T_NO;
      maybe_reset_fpu_inexact<Temp_Boundary_Type>();
      r = assign_r(t_bound, c_inhomogeneous_term, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      r = neg_assign_r(t_bound, t_bound, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      for (dimension_type i = c_space_dim; i-- > 0; ) {
	if (i == k)
	  continue;
	const Coefficient& a_i = c.coefficient(Variable(i));
	int sgn_a_i = sgn(a_i);
	if (sgn_a_i == 0)
	  continue;
	ITV& x_i = seq[i];
	if (sgn_a_i < 0) {
	  if (x_i.upper_is_unbounded())
	    goto next_k;
	  r = assign_r(t_a, a_i, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  r = assign_r(t_x, x_i.upper(), ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  if (x_i.upper_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	}
	else {
	  assert(sgn_a_i > 0);
	  if (x_i.lower_is_unbounded())
	    goto next_k;
	  r = assign_r(t_a, a_i, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  r = assign_r(t_x, x_i.lower(), ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  if (x_i.lower_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	}
      }
      r = assign_r(t_a, a_k, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      r = div_assign_r(t_bound, t_bound, t_a, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto next_k;

      // Refine the upper bound of seq[k] with t_bound.
      if (open == T_MAYBE
	  && maybe_check_fpu_inexact<Temp_Boundary_Type>() == 1)
	open = T_YES;
      seq[k].upper_narrow(t_bound, open == T_YES);
      reset_empty_up_to_date();
    }
    else {
      assert(sgn_a_k < 0);
      open = (c_type == Constraint::STRICT_INEQUALITY) ? T_YES : T_NO;
      if (open == T_NO)
	maybe_reset_fpu_inexact<Temp_Boundary_Type>();
      r = assign_r(t_bound, c_inhomogeneous_term, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_2;
      r = neg_assign_r(t_bound, t_bound, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_2;
      for (dimension_type i = c_space_dim; i-- > 0; ) {
	if (i == k)
	  continue;
	const Coefficient& a_i = c.coefficient(Variable(i));
	int sgn_a_i = sgn(a_i);
	if (sgn_a_i == 0)
	  continue;
	ITV& x_i = seq[i];
	if (sgn_a_i < 0) {
	  if (x_i.lower_is_unbounded())
	    goto maybe_refine_upper_2;
	  r = assign_r(t_a, a_i, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	  r = assign_r(t_x, x_i.lower(), ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	  if (x_i.lower_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	}
	else {
	  assert(sgn_a_i > 0);
	  if (x_i.upper_is_unbounded())
	    goto maybe_refine_upper_2;
	  r = assign_r(t_a, a_i, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	  r = assign_r(t_x, x_i.upper(), ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	  if (x_i.upper_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto maybe_refine_upper_2;
	}
      }
      r = assign_r(t_a, a_k, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_2;
      r = div_assign_r(t_bound, t_bound, t_a, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto maybe_refine_upper_2;

      // Refine the upper bound of seq[k] with t_bound.
      if (open == T_MAYBE
	  && maybe_check_fpu_inexact<Temp_Boundary_Type>() == 1)
	open = T_YES;
      seq[k].upper_narrow(t_bound, open == T_YES);
      reset_empty_up_to_date();
    maybe_refine_upper_2:
      if (c_type != Constraint::EQUALITY)
	continue;
      open = T_NO;
      maybe_reset_fpu_inexact<Temp_Boundary_Type>();
      r = assign_r(t_bound, c_inhomogeneous_term, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      r = neg_assign_r(t_bound, t_bound, ROUND_UP);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      for (dimension_type i = c_space_dim; i-- > 0; ) {
	if (i == k)
	  continue;
	const Coefficient& a_i = c.coefficient(Variable(i));
	int sgn_a_i = sgn(a_i);
	if (sgn_a_i == 0)
	  continue;
	ITV& x_i = seq[i];
	if (sgn_a_i < 0) {
	  if (x_i.upper_is_unbounded())
	    goto next_k;
	  r = assign_r(t_a, a_i, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  r = assign_r(t_x, x_i.upper(), ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  if (x_i.upper_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	}
	else {
	  assert(sgn_a_i > 0);
	  if (x_i.lower_is_unbounded())
	    goto next_k;
	  r = assign_r(t_a, a_i, ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  r = assign_r(t_x, x_i.lower(), ROUND_DOWN);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	  if (x_i.lower_is_open())
	    open = T_YES;
	  r = sub_mul_assign_r(t_bound, t_a, t_x, ROUND_UP);
	  if (propagate_constraint_check_result(r, open))
	    goto next_k;
	}
      }
      r = assign_r(t_a, a_k, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto next_k;
      r = div_assign_r(t_bound, t_bound, t_a, ROUND_DOWN);
      if (propagate_constraint_check_result(r, open))
	goto next_k;

      // Refine the lower bound of seq[k] with t_bound.
      if (open == T_MAYBE
	  && maybe_check_fpu_inexact<Temp_Boundary_Type>() == 1)
	open = T_YES;
      seq[k].lower_narrow(t_bound, open == T_YES);
      reset_empty_up_to_date();
    }
  next_k:
    ;
  }
}

#else // Alternative implementations for propagate_constraint_no_check.

template <typename ITV>
void
Box<ITV>::propagate_constraint_no_check(const Constraint& c) {
  assert(c.space_dimension() <= space_dimension());

  dimension_type c_space_dim = c.space_dimension();
  ITV k[c_space_dim];
  ITV p[c_space_dim];
  for (dimension_type i = c_space_dim; i-- > 0; ) {
    k[i] = c.coefficient(Variable(i));
    ITV& p_i = p[i];
    p_i = seq[i];
    p_i.mul_assign(p_i, k[i]);
  }
  const Coefficient& inhomogeneous_term = c.inhomogeneous_term();
  for (dimension_type i = c_space_dim; i-- > 0; ) {
    int sgn_coefficient_i = sgn(c.coefficient(Variable(i)));
    if (sgn_coefficient_i == 0)
      continue;
    ITV q(inhomogeneous_term);
    for (dimension_type j = c_space_dim; j-- > 0; ) {
      if (i == j)
	continue;
      q.add_assign(q, p[j]);
    }
    q.div_assign(q, k[i]);
    q.neg_assign(q);
    Relation_Symbol rel;
    switch (c.type()) {
    case Constraint::EQUALITY:
      rel = EQUAL;
      break;
    case Constraint::NONSTRICT_INEQUALITY:
      rel = (sgn_coefficient_i > 0) ? GREATER_OR_EQUAL : LESS_OR_EQUAL;
      break;
    case Constraint::STRICT_INEQUALITY:
      rel = (sgn_coefficient_i > 0) ? GREATER_THAN : LESS_THAN;
      break;
    }
    seq[i].refine_existential(rel, q);
    // FIXME: could/should we exploit the return value of refine_existential,
    //        in case it is available?
    // FIMXE: should we instead be lazy and do not even bother about
    //        the possibility the interval becomes empty apart from setting
    //        empty_up_to_date = false?
    if (seq[i].is_empty()) {
      set_empty();
      break;
    }
  }

  assert(OK());
}

#endif // Alternative implementations for propagate_constraint_no_check.

template <typename ITV>
void
Box<ITV>::propagate_constraints_no_check(const Constraint_System& cs) {
  assert(cs.space_dimension() <= space_dimension());

  bool changed;
  do {
    Sequence copy(seq);
    for (Constraint_System::const_iterator i = cs.begin(),
	   cs_end = cs.end(); i != cs_end; ++i)
      propagate_constraint_no_check(*i);

    // Check if the client has requested abandoning all expensive
    // computations.  If so, the exception specified by the client
    // is thrown now.
    maybe_abandon();

    changed = (copy != seq);
  } while (changed);
}

template <typename ITV>
void
Box<ITV>::affine_image(const Variable var,
                       const Linear_Expression& expr,
                       Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("affine_image(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  const dimension_type space_dim = space_dimension();
  const dimension_type expr_space_dim = expr.space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", "v", var);

  if (is_empty())
    return;

  Tmp_Interval_Type expr_value, temp0, temp1;
  expr_value.assign(expr.inhomogeneous_term());
  for (dimension_type i = expr_space_dim; i-- > 0; ) {
    const Coefficient& coeff = expr.coefficient(Variable(i));
    if (coeff != 0) {
      temp0.assign(coeff);
      temp1.assign(seq[i]);
      temp0.mul_assign(temp0, temp1);
      expr_value.add_assign(expr_value, temp0);
    }
  }
  if (denominator != 1) {
    temp0.assign(denominator);
    expr_value.div_assign(expr_value, temp0);
  }
  seq[var.id()].assign(expr_value);

  assert(OK());
}

template <typename ITV>
void
Box<ITV>::affine_preimage(const Variable var,
                          const Linear_Expression& expr,
                          Coefficient_traits::const_reference
                          denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("affine_preimage(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  const dimension_type x_space_dim = space_dimension();
  const dimension_type expr_space_dim = expr.space_dimension();
  if (x_space_dim < expr_space_dim)
    throw_dimension_incompatible("affine_preimage(v, e, d)", "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (x_space_dim < var_space_dim)
    throw_dimension_incompatible("affine_preimage(v, e, d)", "v", var);

  if (is_empty())
    return;

  const Coefficient& expr_v = expr.coefficient(var);
  const bool invertible = (expr_v != 0);
  if (!invertible) {
    Tmp_Interval_Type expr_value, temp0, temp1;
    expr_value.assign(expr.inhomogeneous_term());
    for (dimension_type i = expr_space_dim; i-- > 0; ) {
      const Coefficient& coeff = expr.coefficient(Variable(i));
      if (coeff != 0) {
	temp0.assign(coeff);
	temp1.assign(seq[i]);
	temp0.mul_assign(temp0, temp1);
	expr_value.add_assign(expr_value, temp0);
      }
    }
    if (denominator != 1) {
      temp0.assign(denominator);
      expr_value.div_assign(expr_value, temp0);
    }
    ITV& x_seq_v = seq[var.id()];
    expr_value.intersect_assign(x_seq_v);
    if (expr_value.is_empty())
      set_empty();
    else
      x_seq_v.assign(UNIVERSE);
  }
  else {
    // The affine transformation is invertible.
    // CHECKME: for efficiency, would it be meaningful to avoid
    // the computation of inverse by partially evaluating the call
    // to affine_image?
    Linear_Expression inverse;
    inverse -= expr;
    inverse += (expr_v + denominator) * var;
    affine_image(var, inverse, expr_v);
  }
  assert(OK());
}

template <typename ITV>
void
Box<ITV>
::bounded_affine_image(const Variable var,
                       const Linear_Expression& lb_expr,
                       const Linear_Expression& ub_expr,
                       Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("bounded_affine_image(v, lb, ub, d)", "d == 0");

  // Dimension-compatibility checks.
  const dimension_type space_dim = space_dimension();
  // The dimension of `lb_expr' and `ub_expr' should not be
  // greater than the dimension of `*this'.
  const dimension_type lb_space_dim = lb_expr.space_dimension();
  if (space_dim < lb_space_dim)
    throw_dimension_incompatible("bounded_affine_image(v, lb, ub, d)",
				 "lb", lb_expr);
  const dimension_type ub_space_dim = ub_expr.space_dimension();
  if (space_dim < ub_space_dim)
    throw_dimension_incompatible("bounded_affine_image(v, lb, ub, d)",
				 "ub", ub_expr);
    // `var' should be one of the dimensions of the box.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", "v", var);

  // Any image of an empty box is empty.
  if (is_empty())
    return;

  // Add the constraint implied by the `lb_expr' and `ub_expr'.
  if (denominator > 0)
    refine_with_constraint(lb_expr <= ub_expr);
  else
    refine_with_constraint(lb_expr >= ub_expr);

  // Check whether `var' occurs in `lb_expr' and/or `ub_expr'.
  if (lb_expr.coefficient(var) == 0) {
    // Here `var' can only occur in `ub_expr'.
    generalized_affine_image(var,
			     LESS_OR_EQUAL,
			     ub_expr,
			     denominator);
    if (denominator > 0)
      refine_with_constraint(lb_expr <= denominator*var);
    else
      refine_with_constraint(denominator*var <= lb_expr);
  }
  else if (ub_expr.coefficient(var) == 0) {
    // Here `var' can only occur in `lb_expr'.
    generalized_affine_image(var,
			     GREATER_OR_EQUAL,
			     lb_expr,
			     denominator);
    if (denominator > 0)
      refine_with_constraint(denominator*var <= ub_expr);
    else
      refine_with_constraint(ub_expr <= denominator*var);
  }
  else {
    // Here `var' occurs in both `lb_expr' and `ub_expr'.  As boxes
    // can only use the non-relational constraints, we find the
    // maximum/minimum values `ub_expr' and `lb_expr' obtain with the
    // box and use these instead of the `ub-expr' and `lb-expr'.
    PPL_DIRTY_TEMP(Coefficient, max_num);
    PPL_DIRTY_TEMP(Coefficient, max_den);
    bool max_included;
    PPL_DIRTY_TEMP(Coefficient, min_num);
    PPL_DIRTY_TEMP(Coefficient, min_den);
    bool min_included;
    ITV& seq_v = seq[var.id()];
    if (maximize(ub_expr, max_num, max_den, max_included)) {
      if (minimize(lb_expr, min_num, min_den, min_included)) {
	// The `ub_expr' has a maximum value and the `lb_expr'
	// has a minimum value for the box.
	// Set the bounds for `var' using the minimum for `lb_expr'.
	min_den *= denominator;
	PPL_DIRTY_TEMP0(mpq_class, q);
	assign_r(q.get_num(), min_num, ROUND_NOT_NEEDED);
	assign_r(q.get_den(), min_den, ROUND_NOT_NEEDED);
	q.canonicalize();
	(denominator > 0)
	  ? seq_v.lower_set(q, !min_included)
	  : seq_v.upper_set(q, !min_included);
	// Now make the maximum of lb_expr the upper bound.  If the
	// maximum is not at a box point, then inequality is strict.
	max_den *= denominator;
	assign_r(q.get_num(), max_num, ROUND_NOT_NEEDED);
	assign_r(q.get_den(), max_den, ROUND_NOT_NEEDED);
	q.canonicalize();
	(denominator > 0)
	  ? seq_v.upper_set(q, !max_included)
	  : seq_v.lower_set(q, !max_included);
      }
      else {
	// The `ub_expr' has a maximum value but the `lb_expr'
	// has no minimum value for the box.
	// Set the bounds for `var' using the maximum for `lb_expr'.
	PPL_DIRTY_TEMP0(mpq_class, q);
	max_den *= denominator;
	assign_r(q.get_num(), max_num, ROUND_NOT_NEEDED);
	assign_r(q.get_den(), max_den, ROUND_NOT_NEEDED);
	q.canonicalize();
	if (denominator > 0) {
	  seq_v.lower_set(UNBOUNDED);
	  seq_v.upper_set(q, !max_included);
	}
	else {
	  seq_v.upper_set(UNBOUNDED);
	  seq_v.lower_set(q, !max_included);
	}
      }
    }
    else if (minimize(lb_expr, min_num, min_den, min_included)) {
	// The `ub_expr' has no maximum value but the `lb_expr'
	// has a minimum value for the box.
	// Set the bounds for `var' using the minimum for `lb_expr'.
	min_den *= denominator;
	PPL_DIRTY_TEMP0(mpq_class, q);
	assign_r(q.get_num(), min_num, ROUND_NOT_NEEDED);
	assign_r(q.get_den(), min_den, ROUND_NOT_NEEDED);
	q.canonicalize();
	if (denominator > 0) {
	  seq_v.upper_set(UNBOUNDED);
	  seq_v.lower_set(q, !min_included);
	}
	else {
	  seq_v.lower_set(UNBOUNDED);
	  seq_v.upper_set(q, !min_included);
	}
    }
    else {
      // The `ub_expr' has no maximum value and the `lb_expr'
      // has no minimum value for the box.
      // So we set the bounds to be unbounded.
      seq_v.upper_set(UNBOUNDED);
      seq_v.lower_set(UNBOUNDED);
    }
  }
  assert(OK());
}

template <typename ITV>
void
Box<ITV>
::bounded_affine_preimage(const Variable var,
                          const Linear_Expression& lb_expr,
                          const Linear_Expression& ub_expr,
                          Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  const dimension_type space_dim = space_dimension();
  if (denominator == 0)
    throw_generic("bounded_affine_preimage(v, lb, ub, d)", "d == 0");

  // Dimension-compatibility checks.
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub, d)",
				 "v", var);
  // The dimension of `lb_expr' and `ub_expr' should not be
  // greater than the dimension of `*this'.
  const dimension_type lb_space_dim = lb_expr.space_dimension();
  if (space_dim < lb_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub)",
				 "lb", lb_expr);
  const dimension_type ub_space_dim = ub_expr.space_dimension();
  if (space_dim < ub_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub)",
				 "ub", ub_expr);

  // Any preimage of an empty polyhedron is empty.
  if (marked_empty())
    return;

  const bool negative_denom = (denominator < 0);
  const Coefficient& lb_var_coeff = lb_expr.coefficient(var);
  const Coefficient& ub_var_coeff = ub_expr.coefficient(var);

  // If the implied constraint between `ub_expr and `lb_expr' is
  // independent of `var', then impose it now.
  if (lb_var_coeff == ub_var_coeff) {
    if (negative_denom)
      refine_with_constraint(lb_expr >= ub_expr);
    else
      refine_with_constraint(lb_expr <= ub_expr);
  }

  ITV& seq_var = seq[var.id()];
  if (!seq_var.is_universe()) {
    // We want to work with a positive denominator,
    // so the sign and its (unsigned) value are separated.
    PPL_DIRTY_TEMP_COEFFICIENT(pos_denominator);
    pos_denominator = denominator;
    if (negative_denom)
      neg_assign(pos_denominator, pos_denominator);
    // Store all the information about the upper and lower bounds
    // for `var' before making this interval unbounded.
    bool open_lower = seq_var.lower_is_open();
    bool unbounded_lower = seq_var.lower_is_unbounded();
    PPL_DIRTY_TEMP0(mpq_class, q_seq_var_lower);
    PPL_DIRTY_TEMP(Coefficient, num_lower);
    PPL_DIRTY_TEMP(Coefficient, den_lower);
    if (!unbounded_lower) {
      assign_r(q_seq_var_lower, seq_var.lower(), ROUND_NOT_NEEDED);
      assign_r(num_lower, q_seq_var_lower.get_num(), ROUND_NOT_NEEDED);
      assign_r(den_lower, q_seq_var_lower.get_den(), ROUND_NOT_NEEDED);
      if (negative_denom)
        neg_assign(den_lower, den_lower);
      num_lower *= pos_denominator;
      seq_var.lower_set(UNBOUNDED);
    }
    bool open_upper = seq_var.upper_is_open();
    bool unbounded_upper = seq_var.upper_is_unbounded();
    PPL_DIRTY_TEMP0(mpq_class, q_seq_var_upper);
    PPL_DIRTY_TEMP(Coefficient, num_upper);
    PPL_DIRTY_TEMP(Coefficient, den_upper);
    if (!unbounded_upper) {
      assign_r(q_seq_var_upper, seq_var.upper(), ROUND_NOT_NEEDED);
      assign_r(num_upper, q_seq_var_upper.get_num(), ROUND_NOT_NEEDED);
      assign_r(den_upper, q_seq_var_upper.get_den(), ROUND_NOT_NEEDED);
      if (negative_denom)
        neg_assign(den_upper, den_upper);
      num_upper *= pos_denominator;
      seq_var.upper_set(UNBOUNDED);
    }

    if (!unbounded_lower) {
      // `lb_expr' is revised by removing the `var' component,
      // multiplying by `-' denominator of the lower bound for `var',
      // and adding the lower bound for `var' to the inhomogeneous term.
      Linear_Expression revised_lb_expr(ub_expr);
      revised_lb_expr -= ub_var_coeff * var;
      PPL_DIRTY_TEMP(Coefficient, d);
      neg_assign(d, den_lower);
      revised_lb_expr *= d;
      revised_lb_expr += num_lower;

      // Find the minimum value for the revised lower bound expression
      // and use this to refine the appropriate bound.
      bool included;
      PPL_DIRTY_TEMP(Coefficient, den);
      if (minimize(revised_lb_expr, num_lower, den, included)) {
        den_lower *= (den * ub_var_coeff);
        PPL_DIRTY_TEMP0(mpq_class, q);
        assign_r(q.get_num(), num_lower, ROUND_NOT_NEEDED);
        assign_r(q.get_den(), den_lower, ROUND_NOT_NEEDED);
        q.canonicalize();
        open_lower |= !included;
        if ((ub_var_coeff >= 0) ? !negative_denom : negative_denom)
          seq_var.lower_narrow(q, open_lower);
        else
          seq_var.upper_narrow(q, open_lower);
        if (seq_var.is_empty()) {
          set_empty();
          return;
        }
      }
    }

    if (!unbounded_upper) {
      // `ub_expr' is revised by removing the `var' component,
      // multiplying by `-' denominator of the upper bound for `var',
      // and adding the upper bound for `var' to the inhomogeneous term.
      Linear_Expression revised_ub_expr(lb_expr);
      revised_ub_expr -= lb_var_coeff * var;
      PPL_DIRTY_TEMP(Coefficient, d);
      neg_assign(d, den_upper);
      revised_ub_expr *= d;
      revised_ub_expr += num_upper;

      // Find the maximum value for the revised upper bound expression
      // and use this to refine the appropriate bound.
      bool included;
      PPL_DIRTY_TEMP(Coefficient, den);
      if (maximize(revised_ub_expr, num_upper, den, included)) {
        den_upper *= (den * lb_var_coeff);
        PPL_DIRTY_TEMP0(mpq_class, q);
        assign_r(q.get_num(), num_upper, ROUND_NOT_NEEDED);
        assign_r(q.get_den(), den_upper, ROUND_NOT_NEEDED);
        q.canonicalize();
        open_upper |= !included;
        if ((lb_var_coeff >= 0) ? !negative_denom : negative_denom)
          seq_var.upper_narrow(q, open_upper);
        else
          seq_var.lower_narrow(q, open_upper);
        if (seq_var.is_empty()) {
          set_empty();
          return;
        }
      }
    }
  }

  // If the implied constraint between `ub_expr and `lb_expr' is
  // dependent on `var', then impose on the new box.
  if (lb_var_coeff != ub_var_coeff) {
    if (denominator > 0)
      refine_with_constraint(lb_expr <= ub_expr);
    else
      refine_with_constraint(lb_expr >= ub_expr);
  }

  assert(OK());
}

template <typename ITV>
void
Box<ITV>
::generalized_affine_image(const Variable var,
                           const Relation_Symbol relsym,
                           const Linear_Expression& expr,
                           Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("generalized_affine_image(v, r, e, d)", "d == 0");

  // Dimension-compatibility checks.
  const dimension_type space_dim = space_dimension();
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)",
				 "e", expr);
  // `var' should be one of the dimensions of the box.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)",
				 "v", var);

  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_generic("generalized_affine_image(v, r, e, d)",
		  "r is the disequality relation symbol");

  // First compute the affine image.
  affine_image(var, expr, denominator);

  if (relsym == EQUAL)
    // The affine relation is indeed an affine function.
    return;

  // Any image of an empty box is empty.
  if (is_empty())
    return;

  ITV& seq_var = seq[var.id()];
  switch (relsym) {
  case LESS_OR_EQUAL:
    seq_var.lower_set(UNBOUNDED);
    break;
  case LESS_THAN:
    seq_var.lower_set(UNBOUNDED);
    if (!seq_var.upper_is_unbounded())
      seq_var.refine_existential(LESS_THAN, seq_var.upper());
    break;
  case GREATER_OR_EQUAL:
    seq_var.upper_set(UNBOUNDED);
    break;
  case GREATER_THAN:
    seq_var.upper_set(UNBOUNDED);
    if (!seq_var.lower_is_unbounded())
      seq_var.refine_existential(GREATER_THAN, seq_var.lower());
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }
  assert(OK());
}

template <typename ITV>
void
Box<ITV>
::generalized_affine_preimage(const Variable var,
                              const Relation_Symbol relsym,
                              const Linear_Expression& expr,
                              Coefficient_traits::const_reference denominator)
{
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("generalized_affine_preimage(v, r, e, d)",
			   "d == 0");

  // Dimension-compatibility checks.
  const dimension_type space_dim = space_dimension();
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("generalized_affine_preimage(v, r, e, d)",
				 "e", expr);
  // `var' should be one of the dimensions of the box.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("generalized_affine_preimage(v, r, e, d)",
				 "v", var);
  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_generic("generalized_affine_preimage(v, r, e, d)",
                  "r is the disequality relation symbol");

  // Check whether the affine relation is indeed an affine function.
  if (relsym == EQUAL) {
    affine_preimage(var, expr, denominator);
    return;
  }

  // Compute the reversed relation symbol to simplify later coding.
  Relation_Symbol reversed_relsym;
  switch (relsym) {
  case LESS_THAN:
    reversed_relsym = GREATER_THAN;
    break;
  case LESS_OR_EQUAL:
    reversed_relsym = GREATER_OR_EQUAL;
    break;
  case GREATER_OR_EQUAL:
    reversed_relsym = LESS_OR_EQUAL;
    break;
  case GREATER_THAN:
    reversed_relsym = LESS_THAN;
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }

  // Check whether the preimage of this affine relation can be easily
  // computed as the image of its inverse relation.
  const Coefficient& var_coefficient = expr.coefficient(var);
  if (var_coefficient != 0) {
    Linear_Expression inverse_expr
      = expr - (denominator + var_coefficient) * var;
    PPL_DIRTY_TEMP_COEFFICIENT(inverse_denominator);
    neg_assign(inverse_denominator, var_coefficient);
    Relation_Symbol inverse_relsym
      = (sgn(denominator) == sgn(inverse_denominator))
      ? relsym : reversed_relsym;
    generalized_affine_image(var, inverse_relsym, inverse_expr,
			     inverse_denominator);
    return;
  }

  // Here `var_coefficient == 0', so that the preimage cannot
  // be easily computed by inverting the affine relation.
  // Shrink the box by adding the constraint induced
  // by the affine relation.
  // First, compute the maximum and minimum value reached by
  // `denominator*var' on the box as we need to use non-relational
  // expressions.
  PPL_DIRTY_TEMP(Coefficient, max_num);
  PPL_DIRTY_TEMP(Coefficient, max_den);
  bool max_included;
  bool bound_above = maximize(denominator*var, max_num, max_den, max_included);
  PPL_DIRTY_TEMP(Coefficient, min_num);
  PPL_DIRTY_TEMP(Coefficient, min_den);
  bool min_included;
  bool bound_below = minimize(denominator*var, min_num, min_den, min_included);
  // Use the correct relation symbol
  const Relation_Symbol corrected_relsym
    = (denominator > 0) ? relsym : reversed_relsym;
  // Revise the expression to take into account the denominator of the
  // maximum/minimim value for `var'.
  PPL_DIRTY_TEMP(Linear_Expression, revised_expr);
  dimension_type dim = space_dim;
  PPL_DIRTY_TEMP_COEFFICIENT(d);
  if (corrected_relsym == LESS_THAN || corrected_relsym == LESS_OR_EQUAL) {
    if (bound_below) {
      for ( ; dim > 0; dim--) {
        d = min_den * expr.coefficient(Variable(dim - 1));
        revised_expr += d * Variable(dim - 1);
      }
    }
  }
  else {
    if (bound_above) {
      for ( ; dim > 0; dim--) {
        d = max_den * expr.coefficient(Variable(dim - 1));
        revised_expr += d * Variable(dim - 1);
      }
    }
  }

  switch (corrected_relsym) {
  case LESS_THAN:
    if (bound_below)
      refine_with_constraint(min_num < revised_expr);
    break;
  case LESS_OR_EQUAL:
    if (bound_below)
      (min_included)
        ? refine_with_constraint(min_num <= revised_expr)
        : refine_with_constraint(min_num < revised_expr);
    break;
  case GREATER_OR_EQUAL:
    if (bound_above)
      (max_included)
        ? refine_with_constraint(max_num >= revised_expr)
        : refine_with_constraint(max_num > revised_expr);
    break;
  case GREATER_THAN:
    if (bound_above)
      refine_with_constraint(max_num > revised_expr);
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }
  // If the shrunk box is empty, its preimage is empty too.
  if (is_empty())
    return;
  ITV& seq_v = seq[var.id()];
  seq_v.lower_set(UNBOUNDED);
  seq_v.upper_set(UNBOUNDED);
  assert(OK());
}

template <typename ITV>
void
Box<ITV>
::generalized_affine_image(const Linear_Expression& lhs,
                           const Relation_Symbol relsym,
                           const Linear_Expression& rhs) {
  // Dimension-compatibility checks.
  // The dimension of `lhs' should not be greater than the dimension
  // of `*this'.
  dimension_type lhs_space_dim = lhs.space_dimension();
  const dimension_type space_dim = space_dimension();
  if (space_dim < lhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e1", lhs);
  // The dimension of `rhs' should not be greater than the dimension
  // of `*this'.
  const dimension_type rhs_space_dim = rhs.space_dimension();
  if (space_dim < rhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e2", rhs);

  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_generic("generalized_affine_image(e1, r, e2)",
                  "r is the disequality relation symbol");

  // Any image of an empty box is empty.
  if (marked_empty())
    return;

  // Compute the maximum and minimum value reached by the rhs on the box.
  PPL_DIRTY_TEMP(Coefficient, max_num);
  PPL_DIRTY_TEMP(Coefficient, max_den);
  bool max_included;
  bool max_rhs = maximize(rhs, max_num, max_den, max_included);
  PPL_DIRTY_TEMP(Coefficient, min_num);
  PPL_DIRTY_TEMP(Coefficient, min_den);
  bool min_included;
  bool min_rhs = minimize(rhs, min_num, min_den, min_included);

  // Check whether there is 0, 1 or more than one variable in the lhs
  // and record the variable with the highest dimension; set the box
  // intervals to be unbounded for all other dimensions with non-zero
  // coefficients in the lhs.
  bool has_var = false;
  bool has_more_than_one_var = false;
  // Initialization is just to avoid an annoying warning.
  dimension_type has_var_id = 0;
  for ( ; lhs_space_dim > 0; --lhs_space_dim)
    if (lhs.coefficient(Variable(lhs_space_dim - 1)) != 0) {
      if (has_var) {
        ITV& seq_i = seq[lhs_space_dim - 1];
        seq_i.lower_set(UNBOUNDED);
        seq_i.upper_set(UNBOUNDED);
        has_more_than_one_var = true;
      }
      else {
        has_var = true;
        has_var_id = lhs_space_dim - 1;
      }
    }

  if (has_more_than_one_var) {
    // There is more than one dimension with non-zero coefficient, so
    // we cannot have any information about the dimensions in the lhs.
    // Since all but the highest dimension with non-zero coefficient
    // in the lhs have been set unbounded, it remains to set the
    // highest dimension in the lhs unbounded.
    ITV& seq_var = seq[has_var_id];
    seq_var.lower_set(UNBOUNDED);
    seq_var.upper_set(UNBOUNDED);
    assert(OK());
    return;
  }

  if (has_var) {
    // There is exactly one dimension with non-zero coefficient.
    ITV& seq_var = seq[has_var_id];

    // Compute the new bounds for this dimension defined by the rhs
    // expression.
    const Coefficient& inhomo = lhs.inhomogeneous_term();
    const Coefficient& coeff = lhs.coefficient(Variable(has_var_id));
    PPL_DIRTY_TEMP0(mpq_class, q_max);
    PPL_DIRTY_TEMP0(mpq_class, q_min);
    if (max_rhs) {
      max_num -= inhomo * max_den;
      max_den *= coeff;
      assign_r(q_max.get_num(), max_num, ROUND_NOT_NEEDED);
      assign_r(q_max.get_den(), max_den, ROUND_NOT_NEEDED);
      q_max.canonicalize();
    }
    if (min_rhs) {
      min_num -= inhomo * min_den;
      min_den *= coeff;
      assign_r(q_min.get_num(), min_num, ROUND_NOT_NEEDED);
      assign_r(q_min.get_den(), min_den, ROUND_NOT_NEEDED);
      q_min.canonicalize();
    }

    // The choice as to which bounds should be set depends on the sign
    // of the coefficient of the dimension `has_var_id' in the lhs.
    if (coeff > 0)
      // The coefficient of the dimension in the lhs is +ve.
      switch (relsym) {
      case LESS_OR_EQUAL:
        seq_var.lower_set(UNBOUNDED);
        max_rhs
          ? seq_var.upper_set(q_max, !max_included)
          : seq_var.upper_set(UNBOUNDED);
        break;
      case LESS_THAN:
        seq_var.lower_set(UNBOUNDED);
        max_rhs
          ? seq_var.upper_set(q_max, true)
          : seq_var.upper_set(UNBOUNDED);
        break;
      case EQUAL:
        max_rhs
          ? seq_var.upper_set(q_max, !max_included)
          : seq_var.upper_set(UNBOUNDED);
          min_rhs
            ? seq_var.lower_set(q_min, !min_included)
            : seq_var.lower_set(UNBOUNDED);
          break;
      case GREATER_OR_EQUAL:
        seq_var.upper_set(UNBOUNDED);
        min_rhs
          ? seq_var.lower_set(q_min, !min_included)
          : seq_var.lower_set(UNBOUNDED);
        break;
      case GREATER_THAN:
        seq_var.upper_set(UNBOUNDED);
        min_rhs
          ? seq_var.lower_set(q_min, true)
          : seq_var.lower_set(UNBOUNDED);
        break;
      default:
        // The NOT_EQUAL case has been already dealt with.
        throw std::runtime_error("PPL internal error");
      }
    else
      // The coefficient of the dimension in the lhs is -ve.
      switch (relsym) {
      case GREATER_OR_EQUAL:
        seq_var.lower_set(UNBOUNDED);
        min_rhs
          ? seq_var.upper_set(q_min, !min_included)
          : seq_var.upper_set(UNBOUNDED);
        break;
      case GREATER_THAN:
        seq_var.lower_set(UNBOUNDED);
        min_rhs
          ? seq_var.upper_set(q_min, true)
          : seq_var.upper_set(UNBOUNDED);
        break;
      case EQUAL:
        max_rhs
          ? seq_var.lower_set(q_max, !max_included)
          : seq_var.lower_set(UNBOUNDED);
          min_rhs
            ? seq_var.upper_set(q_min, !min_included)
            : seq_var.upper_set(UNBOUNDED);
          break;
      case LESS_OR_EQUAL:
        seq_var.upper_set(UNBOUNDED);
        max_rhs
          ? seq_var.lower_set(q_max, !max_included)
          : seq_var.lower_set(UNBOUNDED);
        break;
      case LESS_THAN:
        seq_var.upper_set(UNBOUNDED);
        max_rhs
          ? seq_var.lower_set(q_max, true)
          : seq_var.lower_set(UNBOUNDED);
        break;
      default:
        // The NOT_EQUAL case has been already dealt with.
        throw std::runtime_error("PPL internal error");
      }
  }

  else {
    // The lhs is a constant value, so we just need to add the
    // appropriate constraint.
    const Coefficient& inhomo = lhs.inhomogeneous_term();
    switch (relsym) {
    case LESS_THAN:
      refine_with_constraint(inhomo < rhs);
      break;
    case LESS_OR_EQUAL:
      refine_with_constraint(inhomo <= rhs);
      break;
    case EQUAL:
      refine_with_constraint(inhomo == rhs);
      break;
    case GREATER_OR_EQUAL:
      refine_with_constraint(inhomo >= rhs);
      break;
    case GREATER_THAN:
      refine_with_constraint(inhomo > rhs);
      break;
    default:
      // The NOT_EQUAL case has been already dealt with.
      throw std::runtime_error("PPL internal error");
    }
  }
  assert(OK());
}

template <typename ITV>
void
Box<ITV>::generalized_affine_preimage(const Linear_Expression& lhs,
                                      const Relation_Symbol relsym,
                                      const Linear_Expression& rhs) {
  // Dimension-compatibility checks.
  // The dimension of `lhs' should not be greater than the dimension
  // of `*this'.
  dimension_type lhs_space_dim = lhs.space_dimension();
  const dimension_type space_dim = space_dimension();
  if (space_dim < lhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e1", lhs);
  // The dimension of `rhs' should not be greater than the dimension
  // of `*this'.
  const dimension_type rhs_space_dim = rhs.space_dimension();
  if (space_dim < rhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e2", rhs);

  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_generic("generalized_affine_image(e1, r, e2)",
                  "r is the disequality relation symbol");

  // Any image of an empty box is empty.
  if (marked_empty())
    return;

  // For any dimension occurring in the lhs, swap and change the sign
  // of this component for the rhs and lhs.  Then use these in a call
  // to generalized_affine_image/3.
  Linear_Expression revised_lhs = lhs;
  Linear_Expression revised_rhs = rhs;
  for (dimension_type d = lhs_space_dim; d-- > 0; ) {
    const Variable& var = Variable(d);
    if (lhs.coefficient(var) != 0) {
      PPL_DIRTY_TEMP(Coefficient, temp);
      temp = rhs.coefficient(var) + lhs.coefficient(var);
      revised_rhs -= temp * var;
      revised_lhs -= temp * var;
    }
  }
  generalized_affine_image(revised_lhs, relsym, revised_rhs);
  assert(OK());
}

template <typename ITV>
template <typename Iterator>
void
Box<ITV>::CC76_widening_assign(const Box& y, Iterator first, Iterator last) {
  if (y.is_empty())
    return;

  for (dimension_type i = seq.size(); i-- > 0; )
    seq[i].CC76_widening_assign(y.seq[i], first, last);

  assert(OK());
}

template <typename ITV>
void
Box<ITV>::CC76_widening_assign(const Box& y, unsigned* tp) {
  static typename ITV::boundary_type stop_points[] = {
    typename ITV::boundary_type(-2),
    typename ITV::boundary_type(-1),
    typename ITV::boundary_type(0),
    typename ITV::boundary_type(1),
    typename ITV::boundary_type(2)
  };

  Box& x = *this;
  // If there are tokens available, work on a temporary copy.
  if (tp != 0 && *tp > 0) {
    Box<ITV> x_tmp(x);
    x_tmp.CC76_widening_assign(y, 0);
    // If the widening was not precise, use one of the available tokens.
    if (!x.contains(x_tmp))
      --(*tp);
    return;
  }
  x.CC76_widening_assign(y,
			 stop_points,
			 stop_points
			 + sizeof(stop_points)/sizeof(stop_points[0]));
}

template <typename ITV>
void
Box<ITV>::get_limiting_box(const Constraint_System& cs,
                           Box& limiting_box) const {
  const dimension_type cs_space_dim = cs.space_dimension();
  // Private method: the caller has to ensure the following.
  assert(cs_space_dim <= space_dimension());

  for (Constraint_System::const_iterator cs_i = cs.begin(),
         cs_end = cs.end(); cs_i != cs_end; ++cs_i) {
    const Constraint& c = *cs_i;
    dimension_type c_num_vars = 0;
    dimension_type c_only_var = 0;
    // Constraints that are not interval constraints are ignored.
    if (!extract_interval_constraint(c, cs_space_dim, c_num_vars, c_only_var))
      continue;
    // Trivial constraints are ignored.
    if (c_num_vars != 0) {
      // c is a non-trivial interval constraint.
      // add interval constraint to limiting box
      const Coefficient& n = c.inhomogeneous_term();
      const Coefficient& d = c.coefficient(Variable(c_only_var));
      if (interval_relation(seq[c_only_var], c.type(), n, d)
          == Poly_Con_Relation::is_included())
        limiting_box.add_interval_constraint_no_check(c_only_var, c.type(),
                                                      n, d);
    }
  }
}

template <typename ITV>
void
Box<ITV>::limited_CC76_extrapolation_assign(const Box& y,
                                            const Constraint_System& cs,
                                            unsigned* tp) {
  Box& x = *this;
  const dimension_type space_dim = x.space_dimension();

  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    throw_dimension_incompatible("limited_CC76_extrapolation_assign(y, cs)",
                                 y);
  // `cs' must be dimension-compatible with the two boxes.
  const dimension_type cs_space_dim = cs.space_dimension();
  if (space_dim < cs_space_dim)
    throw_constraint_incompatible("limited_CC76_extrapolation_assign(y, cs)");

  // The limited CC76-extrapolation between two boxes in a
  // zero-dimensional space is also a zero-dimensional box
  if (space_dim == 0)
    return;

#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const Box x_copy = *this;
    const Box y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty())
    return;
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  // Build a limiting box using all the constraints in cs
  // that are satisfied by *this.
  Box limiting_box(space_dim, UNIVERSE);
  get_limiting_box(cs, limiting_box);

  x.CC76_widening_assign(y, tp);

  // Intersect the widened box with the limiting box.
  intersection_assign(limiting_box);
}

template <typename ITV>
void
Box<ITV>::CC76_narrowing_assign(const Box& y) {
  const dimension_type space_dim = space_dimension();

  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    throw_dimension_incompatible("CC76_narrowing_assign(y)", y);

#ifndef NDEBUG
  {
    // We assume that `*this' is contained in or equal to `y'.
    const Box x_copy = *this;
    const Box y_copy = y;
    assert(y_copy.contains(x_copy));
  }
#endif

  // If both boxes are zero-dimensional,
  // since `y' contains `*this', we simply return `*this'.
  if (space_dim == 0)
    return;

  // If `y' is empty, since `y' contains `this', `*this' is empty too.
  if (y.is_empty())
    return;
  // If `*this' is empty, we return.
  if (is_empty())
    return;

  // Replace each constraint in `*this' by the corresponding constraint
  // in `y' if the corresponding inhomogeneous terms are both finite.
  for (dimension_type i = space_dim; i-- > 0; ) {
    ITV& x_i = seq[i];
    const ITV& y_i = y.seq[i];
    if (!x_i.lower_is_unbounded()
	&& !y_i.lower_is_unbounded()
	&& x_i.lower() != y_i.lower())
      x_i.lower() = y_i.lower();
    if (!x_i.upper_is_unbounded()
	&& !y_i.upper_is_unbounded()
	&& x_i.upper() != y_i.upper())
      x_i.upper() = y_i.upper();
  }
  assert(OK());
}

template <typename ITV>
Constraint_System
Box<ITV>::constraints() const {
  Constraint_System cs;
  const dimension_type space_dim = space_dimension();
  if (space_dim == 0) {
    if (marked_empty())
      cs = Constraint_System::zero_dim_empty();
  }
  else if (marked_empty())
    cs.insert(0*Variable(space_dim-1) <= -1);
  else {
    // KLUDGE: in the future `cs' will be constructed of the right dimension.
    // For the time being, we force the dimension with the following line.
    cs.insert(0*Variable(space_dim-1) <= 0);

    for (dimension_type k = 0; k < space_dim; ++k) {
      bool closed = false;
      PPL_DIRTY_TEMP(Coefficient, n);
      PPL_DIRTY_TEMP(Coefficient, d);
      if (get_lower_bound(k, closed, n, d)) {
	if (closed)
	  cs.insert(d*Variable(k) >= n);
	else
	  cs.insert(d*Variable(k) > n);
      }
      if (get_upper_bound(k, closed, n, d)) {
	if (closed)
	  cs.insert(d*Variable(k) <= n);
	else
	  cs.insert(d*Variable(k) < n);
      }
    }
  }
  return cs;
}

template <typename ITV>
Constraint_System
Box<ITV>::minimized_constraints() const {
  Constraint_System cs;
  const dimension_type space_dim = space_dimension();
  if (space_dim == 0) {
    if (marked_empty())
      cs = Constraint_System::zero_dim_empty();
  }
  // Make sure emptiness is detected.
  else if (is_empty())
    cs.insert(0*Variable(space_dim-1) <= -1);
  else {
    // KLUDGE: in the future `cs' will be constructed of the right dimension.
    // For the time being, we force the dimension with the following line.
    cs.insert(0*Variable(space_dim-1) <= 0);

    for (dimension_type k = 0; k < space_dim; ++k) {
      bool closed = false;
      PPL_DIRTY_TEMP(Coefficient, n);
      PPL_DIRTY_TEMP(Coefficient, d);
      if (get_lower_bound(k, closed, n, d)) {
	if (closed)
	  // Make sure equality constraints are detected.
	  if (seq[k].is_singleton()) {
	    cs.insert(d*Variable(k) == n);
	    continue;
	  }
	  else
	    cs.insert(d*Variable(k) >= n);
	else
	  cs.insert(d*Variable(k) > n);
      }
      if (get_upper_bound(k, closed, n, d)) {
	if (closed)
	  cs.insert(d*Variable(k) <= n);
	else
	  cs.insert(d*Variable(k) < n);
      }
    }
  }
  return cs;
}

template <typename ITV>
Congruence_System
Box<ITV>::congruences() const {
  Congruence_System cgs;
  const dimension_type space_dim = space_dimension();
  if (space_dim == 0) {
    if (marked_empty())
      cgs = Congruence_System::zero_dim_empty();
  }
  // Make sure emptiness is detected.
  else if (is_empty())
    cgs.insert((0*Variable(space_dim-1) %= -1) / 0);
  else {
    // KLUDGE: in the future `cgs' will be constructed of the right dimension.
    // For the time being, we force the dimension with the following line.
    cgs.insert(0*Variable(space_dim-1) %= 0);

    for (dimension_type k = 0; k < space_dim; ++k) {
      bool closed = false;
      PPL_DIRTY_TEMP(Coefficient, n);
      PPL_DIRTY_TEMP(Coefficient, d);
      if (get_lower_bound(k, closed, n, d) && closed)
	  // Make sure equality congruences are detected.
	  if (seq[k].is_singleton())
	    cgs.insert((d*Variable(k) %= n) / 0);
    }
  }
  return cgs;
}

template <typename ITV>
memory_size_type
Box<ITV>::external_memory_in_bytes() const {
  memory_size_type n = seq.capacity() * sizeof(ITV);
  for (dimension_type k = seq.size(); k-- > 0; )
    n += seq[k].external_memory_in_bytes();
  return n;
}

/*! \relates Parma_Polyhedra_Library::Box */
template <typename ITV>
std::ostream&
IO_Operators::operator<<(std::ostream& s, const Box<ITV>& box) {
  if (box.is_empty())
    s << "false";
  else if (box.is_universe())
    s << "true";
  else
    for (dimension_type k = 0,
	   space_dim = box.space_dimension(); k < space_dim; ) {
      s << Variable(k) << " in " << box[k];
      ++k;
      if (k < space_dim)
	s << ", ";
      else
	break;
    }
  return s;
}

template <typename ITV>
void
Box<ITV>::ascii_dump(std::ostream& s) const {
  const char separator = ' ';
  status.ascii_dump(s);
  const dimension_type space_dim = space_dimension();
  s << "space_dim" << separator << space_dim;
  s << "\n";
  for (dimension_type i = 0; i < space_dim;  ++i)
    seq[i].ascii_dump(s);
}

PPL_OUTPUT_TEMPLATE_DEFINITIONS(ITV, Box<ITV>)

template <typename ITV>
bool
Box<ITV>::ascii_load(std::istream& s) {
  if (!status.ascii_load(s))
    return false;

  std::string str;
  dimension_type space_dim;
  if (!(s >> str) || str != "space_dim")
    return false;
  if (!(s >> space_dim))
    return false;

  seq.clear();
  ITV seq_i;
  for (dimension_type i = 0; i < space_dim;  ++i) {
    if (seq_i.ascii_load(s))
      seq.push_back(seq_i);
    else
      return false;
  }

  // Check invariants.
  assert(OK());
  return true;
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Box& y) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << this->space_dimension()
    << ", y->space_dimension() == " << y.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>
::throw_dimension_incompatible(const char* method,
			       dimension_type required_dim) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", required dimension == " << required_dim << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Constraint& c) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", c->space_dimension == " << c.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Congruence& cg) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", cg->space_dimension == " << cg.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Constraint_System& cs) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", cs->space_dimension == " << cs.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Congruence_System& cgs) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", cgs->space_dimension == " << cgs.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const Generator& g) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", g->space_dimension == " << g.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_constraint_incompatible(const char* method) {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "the constraint is incompatible.";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_expression_too_complex(const char* method,
                                       const Linear_Expression& e) {
  using namespace IO_Operators;
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << e << " is too complex.";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_dimension_incompatible(const char* method,
                                       const char* name_row,
                                       const Linear_Expression& e) const {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", " << name_row << "->space_dimension() == "
    << e.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_generic(const char* method, const char* reason) {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << reason;
  throw std::invalid_argument(s.str());
}

template <typename ITV>
void
Box<ITV>::throw_space_dimension_overflow(const char* method,
                                         const char* reason) {
  std::ostringstream s;
  s << "PPL::Box::" << method << ":" << std::endl
    << reason;
  throw std::length_error(s.str());
}

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
/*! \relates Box */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename Specialization,
	  typename Temp, typename To, typename ITV>
bool
l_m_distance_assign(Checked_Number<To, Extended_Number_Policy>& r,
		    const Box<ITV>& x, const Box<ITV>& y,
		    const Rounding_Dir dir,
		    Temp& tmp0, Temp& tmp1, Temp& tmp2) {
  const dimension_type x_space_dim = x.space_dimension();
  // Dimension-compatibility check.
  if (x_space_dim != y.space_dimension())
    return false;

  // Zero-dim boxes are equal if and only if they are both empty or universe.
  if (x_space_dim == 0) {
    if (x.marked_empty() == y.marked_empty())
      assign_r(r, 0, ROUND_NOT_NEEDED);
    else
      assign_r(r, PLUS_INFINITY, ROUND_NOT_NEEDED);
    return true;
  }

  // The distance computation requires a check for emptiness.
  (void) x.is_empty();
  (void) y.is_empty();
  // If one of two boxes is empty, then they are equal if and only if
  // the other box is empty too.
  if (x.marked_empty() || y.marked_empty()) {
    if (x.marked_empty() == y.marked_empty()) {
      assign_r(r, 0, ROUND_NOT_NEEDED);
      return true;
    }
    else
      goto pinf;
  }

  assign_r(tmp0, 0, ROUND_NOT_NEEDED);
  for (dimension_type i = x_space_dim; i-- > 0; ) {
    const ITV& x_i = x.seq[i];
    const ITV& y_i = y.seq[i];
    // Dealing with the lower bounds.
    if (x_i.lower_is_unbounded()) {
      if (!y_i.lower_is_unbounded())
	goto pinf;
    }
    else if (y_i.lower_is_unbounded())
      goto pinf;
    else {
      const Temp* tmp1p;
      const Temp* tmp2p;
      if (x_i.lower() > y_i.lower()) {
	maybe_assign(tmp1p, tmp1, x_i.lower(), dir);
	maybe_assign(tmp2p, tmp2, y_i.lower(), inverse(dir));
      }
      else {
	maybe_assign(tmp1p, tmp1, y_i.lower(), dir);
	maybe_assign(tmp2p, tmp2, x_i.lower(), inverse(dir));
      }
      sub_assign_r(tmp1, *tmp1p, *tmp2p, dir);
      assert(sgn(tmp1) >= 0);
      Specialization::combine(tmp0, tmp1, dir);
    }
    // Dealing with the lower bounds.
    if (x_i.upper_is_unbounded())
      if (y_i.upper_is_unbounded())
	continue;
      else
	goto pinf;
    else if (y_i.upper_is_unbounded())
      goto pinf;
    else {
      const Temp* tmp1p;
      const Temp* tmp2p;
      if (x_i.upper() > y_i.upper()) {
	maybe_assign(tmp1p, tmp1, x_i.upper(), dir);
	maybe_assign(tmp2p, tmp2, y_i.upper(), inverse(dir));
      }
      else {
	maybe_assign(tmp1p, tmp1, y_i.upper(), dir);
	maybe_assign(tmp2p, tmp2, x_i.upper(), inverse(dir));
      }
      sub_assign_r(tmp1, *tmp1p, *tmp2p, dir);
      assert(sgn(tmp1) >= 0);
      Specialization::combine(tmp0, tmp1, dir);
    }
  }
  Specialization::finalize(tmp0, dir);
  assign_r(r, tmp0, dir);
  return true;

 pinf:
  assign_r(r, PLUS_INFINITY, ROUND_NOT_NEEDED);
  return true;
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Box_templates_hh)
