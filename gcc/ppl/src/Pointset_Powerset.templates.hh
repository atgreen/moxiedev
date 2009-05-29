/* Pointset_Powerset class implementation: non-inline template functions.
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

#ifndef PPL_Pointset_Powerset_templates_hh
#define PPL_Pointset_Powerset_templates_hh 1

#include "Constraint.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "C_Polyhedron.defs.hh"
#include "NNC_Polyhedron.defs.hh"
#include "Variables_Set.defs.hh"
#include <algorithm>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Parma_Polyhedra_Library {

template <typename PS>
void
Pointset_Powerset<PS>::add_disjunct(const PS& ph) {
  Pointset_Powerset& x = *this;
  if (x.space_dimension() != ph.space_dimension()) {
    std::ostringstream s;
    s << "PPL::Pointset_Powerset<PS>::add_disjunct(ph):\n"
      << "this->space_dimension() == " << x.space_dimension() << ", "
      << "ph.space_dimension() == " << ph.space_dimension() << ".";
    throw std::invalid_argument(s.str());
  }
  x.sequence.push_back(Determinate<PS>(ph));
  x.reduced = false;
  assert(x.OK());
}

template <>
template <typename QH>
Pointset_Powerset<NNC_Polyhedron>
::Pointset_Powerset(const Pointset_Powerset<QH>& y,
                    Complexity_Class complexity)
  : Base(), space_dim(y.space_dimension()) {
  Pointset_Powerset& x = *this;
  for (typename Pointset_Powerset<QH>::const_iterator i = y.begin(),
	 y_end = y.end(); i != y_end; ++i)
    x.sequence.push_back(Determinate<NNC_Polyhedron>
			 (NNC_Polyhedron(i->element(), complexity)));

  // FIXME: If the domain elements can be represented _exactly_ as NNC
  // polyhedra, then having x.reduced = y.reduced is correct. This is
  // the case if the domains are both linear and convex which holds
  // for all the currently supported instantiations except for
  // Grids; for this reason the Grid specialization has a
  // separate implementation.  For any non-linear or non-convex
  // domains (e.g., a domain of Intervals with restrictions or a
  // domain of circles) that may be supported in the future, the
  // assignment x.reduced = y.reduced will be a bug.
  x.reduced = y.reduced;

  assert(x.OK());
}

template <typename PS>
template <typename QH>
Pointset_Powerset<PS>
::Pointset_Powerset(const Pointset_Powerset<QH>& y,
                    Complexity_Class complexity)
  : Base(), space_dim(y.space_dimension()) {
  Pointset_Powerset& x = *this;
  for (typename Pointset_Powerset<QH>::const_iterator i = y.begin(),
	 y_end = y.end(); i != y_end; ++i)
    x.sequence.push_back(Determinate<PS>(PS(i->element(), complexity)));
  // Note: this might be non-reduced even when `y' is known to be
  // omega-reduced, because the constructor of PS may have made
  // different QH elements to become comparable.
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::concatenate_assign(const Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;
  // Ensure omega-reduction here, since what follows has quadratic complexity.
  x.omega_reduce();
  y.omega_reduce();
  Pointset_Powerset<PS> new_x(x.space_dim + y.space_dim, EMPTY);
  for (const_iterator xi = x.begin(), x_end = x.end(),
	 y_begin = y.begin(), y_end = y.end(); xi != x_end; ) {
    for (const_iterator yi = y_begin; yi != y_end; ++yi) {
      CS zi = *xi;
      zi.concatenate_assign(*yi);
      assert(!zi.is_bottom());
      new_x.sequence.push_back(zi);
    }
    ++xi;
    if (abandon_expensive_computations && xi != x_end && y_begin != y_end) {
      // Hurry up!
      PS xph = xi->element();
      for (++xi; xi != x_end; ++xi)
	xph.upper_bound_assign(xi->element());
      const_iterator yi = y_begin;
      PS yph = yi->element();
      for (++yi; yi != y_end; ++yi)
	yph.upper_bound_assign(yi->element());
      xph.concatenate_assign(yph);
      x.swap(new_x);
      x.add_disjunct(xph);
      assert(x.OK());
      return;
    }
  }
  x.swap(new_x);
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::add_constraint(const Constraint& c) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_constraint(c);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
bool
Pointset_Powerset<PS>::add_constraint_and_minimize(const Constraint& c) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; )
    if (!si->element().add_constraint_and_minimize(c))
      si = x.sequence.erase(si);
    else {
      x.reduced = false;
      ++si;
    }
  assert(x.OK());
  return !x.empty();
}

template <typename PS>
void
Pointset_Powerset<PS>::refine_with_constraint(const Constraint& c) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().refine_with_constraint(c);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::add_constraints(const Constraint_System& cs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_constraints(cs);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
bool
Pointset_Powerset<PS>::
add_constraints_and_minimize(const Constraint_System& cs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; )
    if (!si->element().add_constraints_and_minimize(cs))
      si = x.sequence.erase(si);
    else {
      x.reduced = false;
      ++si;
    }
  assert(x.OK());
  return !x.empty();
}

template <typename PS>
void
Pointset_Powerset<PS>::refine_with_constraints(const Constraint_System& cs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().refine_with_constraints(cs);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::add_congruence(const Congruence& c) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_congruence(c);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::refine_with_congruence(const Congruence& cg) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().refine_with_congruence(cg);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
bool
Pointset_Powerset<PS>::add_congruence_and_minimize(const Congruence& c) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; )
    if (!si->element().add_congruence_and_minimize(c))
      si = x.sequence.erase(si);
    else {
      x.reduced = false;
      ++si;
    }
  assert(x.OK());
  return !x.empty();
}

template <typename PS>
void
Pointset_Powerset<PS>::add_congruences(const Congruence_System& cs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_congruences(cs);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::refine_with_congruences(const Congruence_System& cgs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().refine_with_congruences(cgs);
  x.reduced = false;
  assert(x.OK());
}

template <typename PS>
bool
Pointset_Powerset<PS>::
add_congruences_and_minimize(const Congruence_System& cs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; )
    if (!si->element().add_congruences_and_minimize(cs))
      si = x.sequence.erase(si);
    else {
      x.reduced = false;
      ++si;
    }
  assert(x.OK());
  return !x.empty();
}

template <typename PS>
void
Pointset_Powerset<PS>::unconstrain(const Variable var) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().unconstrain(var);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::unconstrain(const Variables_Set& to_be_unconstrained) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().unconstrain(to_be_unconstrained);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::add_space_dimensions_and_embed(dimension_type m) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_space_dimensions_and_embed(m);
  x.space_dim += m;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::add_space_dimensions_and_project(dimension_type m) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
	 s_end = x.sequence.end(); si != s_end; ++si)
    si->element().add_space_dimensions_and_project(m);
  x.space_dim += m;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::
remove_space_dimensions(const Variables_Set& to_be_removed) {
  Pointset_Powerset& x = *this;
  Variables_Set::size_type num_removed = to_be_removed.size();
  if (num_removed > 0) {
    for (Sequence_iterator si = x.sequence.begin(),
	   s_end = x.sequence.end(); si != s_end; ++si) {
      si->element().remove_space_dimensions(to_be_removed);
      x.reduced = false;
    }
    x.space_dim -= num_removed;
    assert(x.OK());
  }
}

template <typename PS>
void
Pointset_Powerset<PS>::remove_higher_space_dimensions(dimension_type
						      new_dimension) {
  Pointset_Powerset& x = *this;
  if (new_dimension < x.space_dim) {
    for (Sequence_iterator si = x.sequence.begin(),
	   s_end = x.sequence.end(); si != s_end; ++si) {
      si->element().remove_higher_space_dimensions(new_dimension);
      x.reduced = false;
    }
    x.space_dim = new_dimension;
    assert(x.OK());
  }
}

template <typename PS>
template <typename Partial_Function>
void
Pointset_Powerset<PS>::map_space_dimensions(const Partial_Function& pfunc) {
  Pointset_Powerset& x = *this;
  if (x.is_bottom()) {
    dimension_type n = 0;
    for (dimension_type i = x.space_dim; i-- > 0; ) {
      dimension_type new_i;
      if (pfunc.maps(i, new_i))
	++n;
    }
    x.space_dim = n;
  }
  else {
    Sequence_iterator s_begin = x.sequence.begin();
    for (Sequence_iterator si = s_begin,
	   s_end = x.sequence.end(); si != s_end; ++si)
      si->element().map_space_dimensions(pfunc);
    x.space_dim = s_begin->element().space_dimension();
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::expand_space_dimension(Variable var,
                                              dimension_type m) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    si->element().expand_space_dimension(var, m);
  x.space_dim += m;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::fold_space_dimensions(const Variables_Set& to_be_folded,
                                             Variable var) {
  Pointset_Powerset& x = *this;
  Variables_Set::size_type num_folded = to_be_folded.size();
  if (num_folded > 0) {
    for (Sequence_iterator si = x.sequence.begin(),
           s_end = x.sequence.end(); si != s_end; ++si)
      si->element().fold_space_dimensions(to_be_folded, var);
  }
  x.space_dim -= num_folded;
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::affine_image(Variable var,
                                    const Linear_Expression& expr,
                                    Coefficient_traits::const_reference
                                    denominator) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().affine_image(var, expr, denominator);
    // Note that the underlying domain can apply conservative approximation:
    // that is why it would not be correct to make the loss of reduction
    // conditional on `var' and `expr'.
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>::affine_preimage(Variable var,
                                       const Linear_Expression& expr,
                                       Coefficient_traits::const_reference
                                       denominator) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().affine_preimage(var, expr, denominator);
    // Note that the underlying domain can apply conservative approximation:
    // that is why it would not be correct to make the loss of reduction
    // conditional on `var' and `expr'.
    x.reduced = false;
  }
  assert(x.OK());
}


template <typename PS>
void
Pointset_Powerset<PS>
::generalized_affine_image(const Linear_Expression& lhs,
                           const Relation_Symbol relsym,
                           const Linear_Expression& rhs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().generalized_affine_image(lhs, relsym, rhs);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>
::generalized_affine_preimage(const Linear_Expression& lhs,
                              const Relation_Symbol relsym,
                              const Linear_Expression& rhs) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().generalized_affine_preimage(lhs, relsym, rhs);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>
::generalized_affine_image(Variable var,
                           const Relation_Symbol relsym,
                           const Linear_Expression& expr,
                           Coefficient_traits::const_reference denominator) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().generalized_affine_image(var, relsym, expr, denominator);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>
::generalized_affine_preimage(Variable var,
                              const Relation_Symbol relsym,
                              const Linear_Expression& expr,
                              Coefficient_traits::const_reference denominator) {  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().generalized_affine_preimage(var, relsym, expr, denominator);
    x.reduced = false;
  }
  assert(x.OK());
}


template <typename PS>
void
Pointset_Powerset<PS>
::bounded_affine_image(Variable var,
                       const Linear_Expression& lb_expr,
                       const Linear_Expression& ub_expr,
                       Coefficient_traits::const_reference denominator) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().bounded_affine_image(var, lb_expr, ub_expr, denominator);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
void
Pointset_Powerset<PS>
::bounded_affine_preimage(Variable var,
                          const Linear_Expression& lb_expr,
                          const Linear_Expression& ub_expr,
                          Coefficient_traits::const_reference denominator) {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    si->element().bounded_affine_preimage(var, lb_expr, ub_expr,
                                          denominator);
    x.reduced = false;
  }
  assert(x.OK());
}

template <typename PS>
dimension_type
Pointset_Powerset<PS>::affine_dimension() const {
  // The affine dimension of the powerset is the affine dimension of
  // the smallest vector space in which it can be embedded.
  const Pointset_Powerset& x = *this;
  C_Polyhedron x_ph(space_dim, EMPTY);

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    PS pi(si->element());
    if (!pi.is_empty()) {
      C_Polyhedron phi(space_dim);
      const Constraint_System& cs = pi.minimized_constraints();
      for (Constraint_System::const_iterator i = cs.begin(),
             cs_end = cs.end(); i != cs_end; ++i) {
        const Constraint& c = *i;
        if (c.is_equality())
          phi.add_constraint(c);
      }
      x_ph.poly_hull_assign(phi);
    }
  }

  return x_ph.affine_dimension();
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_universe() const {
  const Pointset_Powerset& x = *this;
  // Exploit omega-reduction, if already computed.
  if (x.is_omega_reduced())
    return x.size() == 1 && x.begin()->element().is_universe();

  // A powerset is universe iff one of its disjuncts is.
  for (const_iterator x_i = x.begin(), x_end = x.end(); x_i != x_end; ++x_i)
    if (x_i->element().is_universe()) {
      // Speculative omega-reduction, if it is worth.
      if (x.size() > 1) {
        Pointset_Powerset<PS> universe(x.space_dimension(), UNIVERSE);
        Pointset_Powerset& xx = const_cast<Pointset_Powerset&>(x);
        xx.swap(universe);
      }
      return true;
    }
  return false;
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_empty() const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().is_empty())
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_discrete() const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().is_discrete())
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_topologically_closed() const {
  const Pointset_Powerset& x = *this;
  // The powerset must be omega-reduced before checking
  // topological closure.
  x.omega_reduce();
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().is_topologically_closed())
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_bounded() const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().is_bounded())
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::constrains(Variable var) const {
  const Pointset_Powerset& x = *this;
  // `var' should be one of the dimensions of the powerset.
  const dimension_type var_space_dim = var.space_dimension();
  if (x.space_dimension() < var_space_dim) {
    std::ostringstream s;
    s << "PPL::Pointset_Powerset<PS>::constrains(v):\n"
      << "this->space_dimension() == " << x.space_dimension() << ", "
      << "v.space_dimension() == " << var_space_dim << ".";
    throw std::invalid_argument(s.str());
  }
  // omega_reduction needed, since a redundant disjunct may constrain var.
  x.omega_reduce();
  // An empty powerset constrains all variables.
  if (x.is_empty())
    return true;
  for (const_iterator x_i = x.begin(), x_end = x.end(); x_i != x_end; ++x_i)
    if (x_i->element().constrains(var))
      return true;
  return false;
}

template <typename PS>
bool
Pointset_Powerset<PS>::is_disjoint_from(const Pointset_Powerset& y) const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = x.sequence.begin(),
         xs_end = x.sequence.end(); si != xs_end; ++si) {
    const PS& pi = si->element();
    for (Sequence_const_iterator sj = y.sequence.begin(),
           ys_end = y.sequence.end(); sj != ys_end; ++sj) {
      const PS& pj = sj->element();
      if (!pi.is_disjoint_from(pj))
        return false;
    }
  }
  return true;
}

template <typename PS>
void
Pointset_Powerset<PS>::topological_closure_assign() {
  Pointset_Powerset& x = *this;
  for (Sequence_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    si->element().topological_closure_assign();
  assert(x.OK());
}

template <typename PS>
bool
Pointset_Powerset<PS>
::intersection_preserving_enlarge_element(PS& to_be_enlarged) const {
  // FIXME: this is just an executable specification.
  const Pointset_Powerset& context = *this;
  assert(context.space_dimension() == to_be_enlarged.space_dimension());
  bool nonempty_intersection = false;
  // TODO: maybe use a *sorted* constraint system?
  PS enlarged(context.space_dimension(), UNIVERSE);
  for (Sequence_const_iterator si = context.sequence.begin(),
         s_end = context.sequence.end(); si != s_end; ++si) {
    PS context_i(si->element());
    context_i.intersection_assign(enlarged);
    PS enlarged_i(to_be_enlarged);
    nonempty_intersection
      |= enlarged_i.simplify_using_context_assign(context_i);
    // TODO: merge the sorted constraints of `enlarged' and `enlarged_i'?
    enlarged.intersection_assign(enlarged_i);
  }
  to_be_enlarged.swap(enlarged);
  return nonempty_intersection;
}

template <typename PS>
bool
Pointset_Powerset<PS>
::simplify_using_context_assign(const Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;

  // Omega reduction is required.
  // TODO: check whether it would be more efficient to Omega-reduce x
  // during the simplification process: when examining *si, we check
  // if it has been made redundant by any of the elements preceding it
  // (which have been already simplified).
  x.omega_reduce();
  if (x.is_empty())
    return false;
  y.omega_reduce();
  if (y.is_empty()) {
    x = y;
    return false;
  }

  if (y.size() == 1) {
    // More efficient, special handling of the singleton context case.
    const PS& y_i = y.sequence.begin()->element();
    for (Sequence_iterator si = x.sequence.begin(),
           s_end = x.sequence.end(); si != s_end; ) {
      PS& x_i = si->element();
      if (x_i.simplify_using_context_assign(y_i))
        ++si;
      else
        // Intersection is empty: drop the disjunct.
        si = x.sequence.erase(si);
    }
  }
  else {
    // The context is not a singleton.
    for (Sequence_iterator si = x.sequence.begin(),
           s_end = x.sequence.end(); si != s_end; ) {
      if (y.intersection_preserving_enlarge_element(si->element()))
        ++si;
      else
        // Intersection with `*si' is empty: drop the disjunct.
        si = x.sequence.erase(si);
    }
  }
  x.reduced = false;
  assert(x.OK());
  return !x.sequence.empty();
}

template <typename PS>
bool
Pointset_Powerset<PS>::contains(const Pointset_Powerset& y) const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = y.sequence.begin(),
         ys_end = y.sequence.end(); si != ys_end; ++si) {
    const PS& pi = si->element();
    bool pi_is_contained = false;
    for (Sequence_const_iterator sj = x.sequence.begin(),
           xs_end = x.sequence.end();
         (sj != xs_end && !pi_is_contained); ++sj) {
      const PS& pj = sj->element();
      if (pj.contains(pi))
        pi_is_contained = true;
    }
    if (!pi_is_contained)
      return false;
  }
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::strictly_contains(const Pointset_Powerset& y) const {
  /* omega reduction ensures that a disjunct of y cannot be strictly
     contained in one disjunct and also contained but not strictly
     contained in another disjunct of *this */
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  for (Sequence_const_iterator si = y.sequence.begin(),
         ys_end = y.sequence.end(); si != ys_end; ++si) {
    const PS& pi = si->element();
    bool pi_is_strictly_contained = false;
    for (Sequence_const_iterator sj = x.sequence.begin(),
           xs_end = x.sequence.end();
         (sj != xs_end && !pi_is_strictly_contained); ++sj) {
      const PS& pj = sj->element();
      if (pj.strictly_contains(pi))
        pi_is_strictly_contained = true;
    }
    if (!pi_is_strictly_contained)
      return false;
  }
  return true;
}

template <typename PS>
Poly_Con_Relation
Pointset_Powerset<PS>::relation_with(const Congruence& cg) const {
  const Pointset_Powerset& x = *this;

  /* *this is included in cg if every disjunct is included in cg */
  bool is_included = true;
  /* *this is disjoint with cg if every disjunct is disjoint with cg */
  bool is_disjoint = true;
  /* *this strictly_intersects with cg if some disjunct strictly
     intersects with cg */
  bool is_strictly_intersecting = false;
  /* *this saturates cg if some disjunct saturates cg and
     every disjunct is either disjoint from cg or saturates cg */
  bool saturates_once = false;
  bool may_saturate = true;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    Poly_Con_Relation relation_i = si->element().relation_with(cg);
    if (!relation_i.implies(Poly_Con_Relation::is_included()))
      is_included = false;
    if (!relation_i.implies(Poly_Con_Relation::is_disjoint()))
      is_disjoint = false;
    if (relation_i.implies(Poly_Con_Relation::strictly_intersects()))
      is_strictly_intersecting = true;
    if (relation_i.implies(Poly_Con_Relation::saturates()))
      saturates_once = true;
    else if (!relation_i.implies(Poly_Con_Relation::is_disjoint()))
      may_saturate = false;
  }

  Poly_Con_Relation result = Poly_Con_Relation::nothing();
  if (is_included)
    result = result && Poly_Con_Relation::is_included();
  if (is_disjoint)
    result = result && Poly_Con_Relation::is_disjoint();
  if (is_strictly_intersecting)
    result = result && Poly_Con_Relation::strictly_intersects();
  if (saturates_once && may_saturate)
    result = result && Poly_Con_Relation::saturates();

  return result;
}

template <typename PS>
Poly_Con_Relation
Pointset_Powerset<PS>::relation_with(const Constraint& c) const {
  const Pointset_Powerset& x = *this;

  /* *this is included in c if every disjunct is included in c */
  bool is_included = true;
  /* *this is disjoint with c if every disjunct is disjoint with c */
  bool is_disjoint = true;
  /* *this strictly_intersects with c if some disjunct strictly
     intersects with c */
  bool is_strictly_intersecting = false;
  /* *this saturates c if some disjunct saturates c and
     every disjunct is either disjoint from c or saturates c */
  bool saturates_once = false;
  bool may_saturate = true;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    Poly_Con_Relation relation_i = si->element().relation_with(c);
    if (!relation_i.implies(Poly_Con_Relation::is_included()))
      is_included = false;
    if (!relation_i.implies(Poly_Con_Relation::is_disjoint()))
      is_disjoint = false;
    if (relation_i.implies(Poly_Con_Relation::strictly_intersects()))
      is_strictly_intersecting = true;
    if (relation_i.implies(Poly_Con_Relation::saturates()))
      saturates_once = true;
    else if (!relation_i.implies(Poly_Con_Relation::is_disjoint()))
      may_saturate = false;
  }

  Poly_Con_Relation result = Poly_Con_Relation::nothing();
  if (is_included)
    result = result && Poly_Con_Relation::is_included();
  if (is_disjoint)
    result = result && Poly_Con_Relation::is_disjoint();
  if (is_strictly_intersecting)
    result = result && Poly_Con_Relation::strictly_intersects();
  if (saturates_once && may_saturate)
    result = result && Poly_Con_Relation::saturates();

  return result;
}

template <typename PS>
Poly_Gen_Relation
Pointset_Powerset<PS>::relation_with(const Generator& g) const {
  const Pointset_Powerset& x = *this;

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    Poly_Gen_Relation relation_i = si->element().relation_with(g);
    if (relation_i.implies(Poly_Gen_Relation::subsumes()))
      return Poly_Gen_Relation::subsumes();
  }

  return Poly_Gen_Relation::nothing();
}

template <typename PS>
bool
Pointset_Powerset<PS>
::bounds_from_above(const Linear_Expression& expr) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().bounds_from_above(expr))
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>
::bounds_from_below(const Linear_Expression& expr) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (!si->element().bounds_from_below(expr))
      return false;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::maximize(const Linear_Expression& expr,
                                Coefficient& sup_n,
                                Coefficient& sup_d,
                                bool& maximum) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  bool first = true;

  PPL_DIRTY_TEMP_COEFFICIENT(supt_n);
  PPL_DIRTY_TEMP_COEFFICIENT(supt_d);
  supt_n = 0;
  supt_d = 1;
  bool maxt = 0;

  PPL_DIRTY_TEMP_COEFFICIENT(supi_n);
  PPL_DIRTY_TEMP_COEFFICIENT(supi_d);
  supi_n = 0;
  supi_d = 1;
  bool maxi = 0;

  PPL_DIRTY_TEMP_COEFFICIENT(tmp);

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    if (!si->element().maximize(expr, supi_n, supi_d, maxi))
      return false;
    else
      if (first) {
        first = false;
        supt_n = supi_n;
        supt_d = supi_d;
        maxt = maxi;
      }
      else {
        tmp = (supt_n * supi_d) - (supi_n * supt_d);
        if (tmp < 0) {
          supt_n = supi_n;
          supt_d = supi_d;
          maxt = maxi;
        }
        else if (tmp == 0)
          maxt = maxt || maxi;
      }
  }
  sup_n = supt_n;
  sup_d = supt_d;
  maximum = maxt;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::maximize(const Linear_Expression& expr,
                                Coefficient& sup_n,
                                Coefficient& sup_d,
                                bool& maximum,
                                Generator& g) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  bool first = true;

  PPL_DIRTY_TEMP_COEFFICIENT(supt_n);
  PPL_DIRTY_TEMP_COEFFICIENT(supt_d);
  supt_n = 0;
  supt_d = 1;
  bool maxt = 0;
  Generator gt = point();

  PPL_DIRTY_TEMP_COEFFICIENT(supi_n);
  PPL_DIRTY_TEMP_COEFFICIENT(supi_d);
  supi_n = 0;
  supi_d = 1;
  bool maxi = 0;
  Generator gi = point();

  PPL_DIRTY_TEMP_COEFFICIENT(tmp);

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    if (!si->element().maximize(expr, supi_n, supi_d, maxi, gi))
      return false;
    else
      if (first) {
        first = false;
        supt_n = supi_n;
        supt_d = supi_d;
        maxt = maxi;
        gt = gi;
      }
      else {
        tmp = (supt_n * supi_d) - (supi_n * supt_d);
        if (tmp < 0) {
          supt_n = supi_n;
          supt_d = supi_d;
          maxt = maxi;
          gt = gi;
        }
        else if (tmp == 0) {
          maxt = maxt || maxi;
          gt = gi;
        }
      }
  }
  sup_n = supt_n;
  sup_d = supt_d;
  maximum = maxt;
  g = gt;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::minimize(const Linear_Expression& expr,
                                Coefficient& inf_n,
                                Coefficient& inf_d,
                                bool& minimum) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  bool first = true;

  PPL_DIRTY_TEMP_COEFFICIENT(inft_n);
  PPL_DIRTY_TEMP_COEFFICIENT(inft_d);
  inft_n = 0;
  inft_d = 1;
  bool mint = 0;

  PPL_DIRTY_TEMP_COEFFICIENT(infi_n);
  PPL_DIRTY_TEMP_COEFFICIENT(infi_d);
  infi_n = 0;
  infi_d = 1;
  bool mini = 0;

  PPL_DIRTY_TEMP_COEFFICIENT(tmp);

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    if (!si->element().minimize(expr, infi_n, infi_d, mini))
      return false;
    else
      if (first) {
        first = false;
        inft_n = infi_n;
        inft_d = infi_d;
        mint = mini;
      }
      else {
        tmp = (inft_n * infi_d) - (infi_n * inft_d);
        if (tmp > 0) {
          inft_n = infi_n;
          inft_d = infi_d;
          mint = mini;
        }
        else if (tmp == 0)
          mint = mint || mini;
      }
  }
  inf_n = inft_n;
  inf_d = inft_d;
  minimum = mint;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::minimize(const Linear_Expression& expr,
                                Coefficient& inf_n,
                                Coefficient& inf_d,
                                bool& minimum,
                                Generator& g) const {
  const Pointset_Powerset& x = *this;
  x.omega_reduce();
  bool first = true;

  PPL_DIRTY_TEMP_COEFFICIENT(inft_n);
  PPL_DIRTY_TEMP_COEFFICIENT(inft_d);
  inft_n = 0;
  inft_d = 1;
  bool mint = 0;
  Generator gt = point();

  PPL_DIRTY_TEMP_COEFFICIENT(infi_n);
  PPL_DIRTY_TEMP_COEFFICIENT(infi_d);
  infi_n = 0;
  infi_d = 1;
  bool mini = 0;
  Generator gi = point();

  PPL_DIRTY_TEMP_COEFFICIENT(tmp);

  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si) {
    if (!si->element().minimize(expr, infi_n, infi_d, mini, gi))
      return false;
    else
      if (first) {
        first = false;
        inft_n = infi_n;
        inft_d = infi_d;
        mint = mini;
        gt = gi;
      }
      else {
        tmp = (inft_n * infi_d) - (infi_n * inft_d);
        if (tmp > 0) {
          inft_n = infi_n;
          inft_d = infi_d;
          mint = mini;
          gt = gi;
        }
        else if (tmp == 0) {
          mint = mint || mini;
          gt = gi;
        }
      }
  }
  inf_n = inft_n;
  inf_d = inft_d;
  minimum = mint;
  g = gt;
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::contains_integer_point() const {
  const Pointset_Powerset& x = *this;
  for (Sequence_const_iterator si = x.sequence.begin(),
         s_end = x.sequence.end(); si != s_end; ++si)
    if (si->element().contains_integer_point())
      return true;
  return false;
}

template <typename PS>
void
Pointset_Powerset<PS>::pairwise_reduce() {
  Pointset_Powerset& x = *this;
  // It is wise to omega-reduce before pairwise-reducing.
  x.omega_reduce();

  size_type n = x.size();
  size_type deleted;
  do {
    Pointset_Powerset new_x(x.space_dim, EMPTY);
    std::deque<bool> marked(n, false);
    deleted = 0;
    Sequence_iterator s_begin = x.sequence.begin();
    Sequence_iterator s_end = x.sequence.end();
    unsigned si_index = 0;
    for (Sequence_iterator si = s_begin; si != s_end; ++si, ++si_index) {
      if (marked[si_index])
	continue;
      PS& pi = si->element();
      Sequence_const_iterator sj = si;
      unsigned sj_index = si_index;
      for (++sj, ++sj_index; sj != s_end; ++sj, ++sj_index) {
	if (marked[sj_index])
	  continue;
	const PS& pj = sj->element();
	if (pi.upper_bound_assign_if_exact(pj)) {
	  marked[si_index] = marked[sj_index] = true;
	  new_x.add_non_bottom_disjunct_preserve_reduction(pi);
	  ++deleted;
	  goto next;
	}
      }
    next:
      ;
    }
    iterator nx_begin = new_x.begin();
    iterator nx_end = new_x.end();
    unsigned xi_index = 0;
    for (const_iterator xi = x.begin(),
	   x_end = x.end(); xi != x_end; ++xi, ++xi_index)
      if (!marked[xi_index])
	nx_begin = new_x.add_non_bottom_disjunct_preserve_reduction(*xi,
								    nx_begin,
								    nx_end);
    std::swap(x.sequence, new_x.sequence);
    n -= deleted;
  } while (deleted > 0);
  assert(x.OK());
}

template <typename PS>
template <typename Widening>
void
Pointset_Powerset<PS>::
BGP99_heuristics_assign(const Pointset_Powerset& y, Widening wf) {
  // `x' is the current iteration value.
  Pointset_Powerset& x = *this;

#ifndef NDEBUG
  {
    // We assume that `y' entails `x'.
    const Pointset_Powerset<PS> x_copy = x;
    const Pointset_Powerset<PS> y_copy = y;
    assert(y_copy.definitely_entails(x_copy));
  }
#endif

  size_type n = x.size();
  Pointset_Powerset new_x(x.space_dim, EMPTY);
  std::deque<bool> marked(n, false);
  const_iterator x_begin = x.begin();
  const_iterator x_end = x.end();
  unsigned i_index = 0;
  for (const_iterator i = x_begin,
	 y_begin = y.begin(), y_end = y.end(); i != x_end; ++i, ++i_index)
    for (const_iterator j = y_begin; j != y_end; ++j) {
      const PS& pi = i->element();
      const PS& pj = j->element();
      if (pi.contains(pj)) {
	PS pi_copy = pi;
	wf(pi_copy, pj);
	new_x.add_non_bottom_disjunct_preserve_reduction(pi_copy);
	marked[i_index] = true;
      }
    }
  iterator nx_begin = new_x.begin();
  iterator nx_end = new_x.end();
  i_index = 0;
  for (const_iterator i = x_begin; i != x_end; ++i, ++i_index)
    if (!marked[i_index])
      nx_begin = new_x.add_non_bottom_disjunct_preserve_reduction(*i,
								  nx_begin,
								  nx_end);
  std::swap(x.sequence, new_x.sequence);
  assert(x.OK());
  assert(x.is_omega_reduced());
}

template <typename PS>
template <typename Widening>
void
Pointset_Powerset<PS>::
BGP99_extrapolation_assign(const Pointset_Powerset& y,
			   Widening wf,
			   unsigned max_disjuncts) {
  // `x' is the current iteration value.
  Pointset_Powerset& x = *this;

#ifndef NDEBUG
  {
    // We assume that `y' entails `x'.
    const Pointset_Powerset<PS> x_copy = x;
    const Pointset_Powerset<PS> y_copy = y;
    assert(y_copy.definitely_entails(x_copy));
  }
#endif

  x.pairwise_reduce();
  if (max_disjuncts != 0)
    x.collapse(max_disjuncts);
  x.BGP99_heuristics_assign(y, wf);
}

template <typename PS>
template <typename Cert>
void
Pointset_Powerset<PS>::
collect_certificates(std::map<Cert, size_type,
                     typename Cert::Compare>& cert_ms) const {
  const Pointset_Powerset& x = *this;
  assert(x.is_omega_reduced());
  assert(cert_ms.size() == 0);
  for (const_iterator i = x.begin(), end = x.end(); i != end; i++) {
    Cert ph_cert(i->element());
    ++cert_ms[ph_cert];
  }
}

template <typename PS>
template <typename Cert>
bool
Pointset_Powerset<PS>::
is_cert_multiset_stabilizing(const std::map<Cert, size_type,
                             typename Cert::Compare>& y_cert_ms
			     ) const {
  typedef std::map<Cert, size_type, typename Cert::Compare> Cert_Multiset;
  Cert_Multiset x_cert_ms;
  collect_certificates(x_cert_ms);
  typename Cert_Multiset::const_iterator
    xi = x_cert_ms.begin(),
    x_cert_ms_end = x_cert_ms.end(),
    yi = y_cert_ms.begin(),
    y_cert_ms_end = y_cert_ms.end();
  while (xi != x_cert_ms_end && yi != y_cert_ms_end) {
    const Cert& xi_cert = xi->first;
    const Cert& yi_cert = yi->first;
    switch (xi_cert.compare(yi_cert)) {
    case 0:
      // xi_cert == yi_cert: check the number of multiset occurrences.
      {
	const size_type& xi_count = xi->second;
	const size_type& yi_count = yi->second;
	if (xi_count == yi_count) {
	  // Same number of occurrences: compare the next pair.
	  ++xi;
	  ++yi;
	}
	else
	  // Different number of occurrences: can decide ordering.
	  return xi_count < yi_count;
	break;
      }
    case 1:
      // xi_cert > yi_cert: it is not stabilizing.
      return false;

    case -1:
      // xi_cert < yi_cert: it is stabilizing.
      return true;
    }
  }
  // Here xi == x_cert_ms_end or yi == y_cert_ms_end.
  // Stabilization is achieved if `y_cert_ms' still has other elements.
  return yi != y_cert_ms_end;
}

template <typename PS>
template <typename Cert, typename Widening>
void
Pointset_Powerset<PS>::BHZ03_widening_assign(const Pointset_Powerset& y,
                                             Widening wf) {
  // `x' is the current iteration value.
  Pointset_Powerset& x = *this;

#ifndef NDEBUG
  {
    // We assume that `y' entails `x'.
    const Pointset_Powerset<PS> x_copy = x;
    const Pointset_Powerset<PS> y_copy = y;
    assert(y_copy.definitely_entails(x_copy));
  }
#endif

  // First widening technique: do nothing.

  // If `y' is the empty collection, do nothing.
  assert(x.size() > 0);
  if (y.size() == 0)
    return;

  // Compute the poly-hull of `x'.
  PS x_hull(x.space_dim, EMPTY);
  for (const_iterator i = x.begin(), x_end = x.end(); i != x_end; ++i)
    x_hull.upper_bound_assign(i->element());

  // Compute the poly-hull of `y'.
  PS y_hull(y.space_dim, EMPTY);
  for (const_iterator i = y.begin(), y_end = y.end(); i != y_end; ++i)
    y_hull.upper_bound_assign(i->element());
  // Compute the certificate for `y_hull'.
  const Cert y_hull_cert(y_hull);

  // If the hull is stabilizing, do nothing.
  int hull_stabilization = y_hull_cert.compare(x_hull);
  if (hull_stabilization == 1)
    return;

  // Multiset ordering is only useful when `y' is not a singleton.
  const bool y_is_not_a_singleton = y.size() > 1;

  // The multiset certificate for `y':
  // we want to be lazy about its computation.
  typedef std::map<Cert, size_type, typename Cert::Compare> Cert_Multiset;
  Cert_Multiset y_cert_ms;
  bool y_cert_ms_computed = false;

  if (hull_stabilization == 0 && y_is_not_a_singleton) {
    // Collect the multiset certificate for `y'.
    y.collect_certificates(y_cert_ms);
    y_cert_ms_computed = true;
    // If multiset ordering is stabilizing, do nothing.
    if (x.is_cert_multiset_stabilizing(y_cert_ms))
      return;
  }

  // Second widening technique: try the BGP99 powerset heuristics.
  Pointset_Powerset<PS> bgp99_heuristics = x;
  bgp99_heuristics.BGP99_heuristics_assign(y, wf);

  // Compute the poly-hull of `bgp99_heuristics'.
  PS bgp99_heuristics_hull(x.space_dim, EMPTY);
  for (const_iterator i = bgp99_heuristics.begin(),
	 bh_end = bgp99_heuristics.end(); i != bh_end; ++i)
    bgp99_heuristics_hull.upper_bound_assign(i->element());

  // Check for stabilization and, if successful,
  // commit to the result of the extrapolation.
  hull_stabilization = y_hull_cert.compare(bgp99_heuristics_hull);
  if (hull_stabilization == 1) {
    // The poly-hull is stabilizing.
    std::swap(x, bgp99_heuristics);
    return;
  }
  else if (hull_stabilization == 0 && y_is_not_a_singleton) {
    // If not already done, compute multiset certificate for `y'.
    if (!y_cert_ms_computed) {
      y.collect_certificates(y_cert_ms);
      y_cert_ms_computed = true;
    }
    if (bgp99_heuristics.is_cert_multiset_stabilizing(y_cert_ms)) {
      std::swap(x, bgp99_heuristics);
      return;
    }
    // Third widening technique: pairwise-reduction on `bgp99_heuristics'.
    // Note that pairwise-reduction does not affect the computation
    // of the poly-hulls, so that we only have to check the multiset
    // certificate relation.
    Pointset_Powerset<PS> reduced_bgp99_heuristics(bgp99_heuristics);
    reduced_bgp99_heuristics.pairwise_reduce();
    if (reduced_bgp99_heuristics.is_cert_multiset_stabilizing(y_cert_ms)) {
      std::swap(x, reduced_bgp99_heuristics);
      return;
    }
  }

  // Fourth widening technique: this is applicable only when
  // `y_hull' is a proper subset of `bgp99_heuristics_hull'.
  if (bgp99_heuristics_hull.strictly_contains(y_hull)) {
    // Compute (y_hull \widen bgp99_heuristics_hull).
    PS ph = bgp99_heuristics_hull;
    wf(ph, y_hull);
    // Compute the difference between `ph' and `bgp99_heuristics_hull'.
    ph.difference_assign(bgp99_heuristics_hull);
    x.add_disjunct(ph);
    return;
  }

  // Fall back to the computation of the poly-hull.
  Pointset_Powerset<PS> x_hull_singleton(x.space_dim, EMPTY);
  x_hull_singleton.add_disjunct(x_hull);
  std::swap(x, x_hull_singleton);
}

template <typename PS>
void
Pointset_Powerset<PS>::ascii_dump(std::ostream& s) const {
  const Pointset_Powerset& x = *this;
  s << "size " << x.size()
    << "\nspace_dim " << x.space_dim
    << "\n";
  for (const_iterator xi = x.begin(), x_end = x.end(); xi != x_end; ++xi)
    xi->element().ascii_dump(s);
}

PPL_OUTPUT_TEMPLATE_DEFINITIONS(PS, Pointset_Powerset<PS>)

  template <typename PS>
bool
Pointset_Powerset<PS>::ascii_load(std::istream& s) {
  Pointset_Powerset& x = *this;
  std::string str;

  if (!(s >> str) || str != "size")
    return false;

  size_type sz;

  if (!(s >> sz))
    return false;

  if (!(s >> str) || str != "space_dim")
    return false;

  if (!(s >> x.space_dim))
    return false;

  Pointset_Powerset new_x(x.space_dim, EMPTY);
  while (sz-- > 0) {
    PS ph;
    if (!ph.ascii_load(s))
      return false;
    new_x.add_disjunct(ph);
  }
  x.swap(new_x);

  // Check invariants.
  assert(x.OK());
  return true;
}

template <typename PS>
bool
Pointset_Powerset<PS>::OK() const {
  const Pointset_Powerset& x = *this;
  for (const_iterator xi = x.begin(), x_end = x.end(); xi != x_end; ++xi) {
    const PS& pi = xi->element();
    if (pi.space_dimension() != x.space_dim) {
#ifndef NDEBUG
      std::cerr << "Space dimension mismatch: is " << pi.space_dimension()
		<< " in an element of the sequence,\nshould be "
		<< x.space_dim << "."
		<< std::endl;
#endif
      return false;
    }
  }
  return x.Base::OK();
}

namespace Implementation {

namespace Pointset_Powersets {

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Partitions polyhedron \p qq according to constraint \p c.
/*! \relates Parma_Polyhedra_Library::Pointset_Powerset
  On exit, the intersection of \p qq and constraint \p c is stored
  in \p qq, whereas the intersection of \p qq with the negation of \p c
  is added as a new disjunct of the powerset \p r.
*/
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename PS>
void
linear_partition_aux(const Constraint& c,
		     PS& qq,
		     Pointset_Powerset<NNC_Polyhedron>& r) {
  Linear_Expression le(c);
  const Constraint& neg_c = c.is_strict_inequality() ? (le <= 0) : (le < 0);
  NNC_Polyhedron qqq(qq);
  qqq.add_constraint(neg_c);
  if (!qqq.is_empty())
    r.add_disjunct(qqq);
  qq.add_constraint(c);
}

} // namespace Pointset_Powersets

} // namespace Implementation


/*! \relates Pointset_Powerset */
template <typename PS>
std::pair<PS, Pointset_Powerset<NNC_Polyhedron> >
linear_partition(const PS& p, const PS& q) {
  using Implementation::Pointset_Powersets::linear_partition_aux;

  Pointset_Powerset<NNC_Polyhedron> r(p.space_dimension(), EMPTY);
  PS qq = q;
  const Constraint_System& pcs = p.constraints();
  for (Constraint_System::const_iterator i = pcs.begin(),
	 pcs_end = pcs.end(); i != pcs_end; ++i) {
    const Constraint& c = *i;
    if (c.is_equality()) {
      Linear_Expression le(c);
      linear_partition_aux(le <= 0, qq, r);
      linear_partition_aux(le >= 0, qq, r);
    }
    else
      linear_partition_aux(c, qq, r);
  }
  return std::make_pair(qq, r);
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Pointset_Powerset_templates_hh)
