/* Pointset_Powerset class implementation: inline functions.
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

#ifndef PPL_Pointset_Powerset_inlines_hh
#define PPL_Pointset_Powerset_inlines_hh 1

#include "Constraint.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "Congruence.defs.hh"
#include "Congruence_System.defs.hh"
#include "Congruence_System.inlines.hh"
#include "C_Polyhedron.defs.hh"
#include "NNC_Polyhedron.defs.hh"
#include <algorithm>
#include <deque>

namespace Parma_Polyhedra_Library {

template <typename PS>
inline dimension_type
Pointset_Powerset<PS>::space_dimension() const {
  return space_dim;
}

template <typename PS>
inline dimension_type
Pointset_Powerset<PS>::max_space_dimension() {
  return PS::max_space_dimension();
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(dimension_type num_dimensions,
					 Degenerate_Element kind)
  : Base(), space_dim(num_dimensions) {
  Pointset_Powerset& x = *this;
  if (kind == UNIVERSE)
    x.sequence.push_back(Determinate<PS>(PS(num_dimensions, kind)));
  assert(x.OK());
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const Pointset_Powerset& y,
                                         Complexity_Class)
  : Base(y), space_dim(y.space_dim) {
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const C_Polyhedron& ph,
                                         Complexity_Class complexity)
  : Base(), space_dim(ph.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (complexity == ANY_COMPLEXITY) {
    if (ph.is_empty())
      return;
  }
  else
    x.reduced = false;
  x.sequence.push_back(Determinate<PS>(PS(ph, complexity)));
  x.reduced = false;
  assert(OK());
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const NNC_Polyhedron& ph,
                                         Complexity_Class complexity)
  : Base(), space_dim(ph.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (complexity == ANY_COMPLEXITY) {
    if (ph.is_empty())
      return;
  }
  else
    x.reduced = false;
  x.sequence.push_back(Determinate<PS>(PS(ph, complexity)));
  assert(OK());
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const Grid& gr,
                                         Complexity_Class)
  : Base(), space_dim(gr.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (!gr.is_empty()) {
    x.sequence.push_back(Determinate<PS>(PS(gr)));
  }
  assert(OK());
}

template <typename PS>
template <typename QH1, typename QH2, typename R>
inline
Pointset_Powerset<PS>
::Pointset_Powerset(const Partially_Reduced_Product<QH1, QH2, R>& prp,
                    Complexity_Class complexity)
  : Base(), space_dim(prp.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (complexity == ANY_COMPLEXITY) {
    if (prp.is_empty())
      return;
  }
  else
    x.reduced = false;
  x.sequence.push_back(Determinate<PS>(PS(prp, complexity)));
  x.reduced = false;
  assert(OK());
}

template <typename PS>
template <typename Interval>
Pointset_Powerset<PS>::Pointset_Powerset(const Box<Interval>& box,
                                         Complexity_Class)
  : Base(), space_dim(box.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (!box.is_empty())
    x.sequence.push_back(Determinate<PS>(PS(box)));
  assert(OK());
}

template <typename PS>
template <typename T>
Pointset_Powerset<PS>::Pointset_Powerset(const Octagonal_Shape<T>& os,
                                         Complexity_Class)
  : Base(), space_dim(os.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (!os.is_empty())
    x.sequence.push_back(Determinate<PS>(PS(os)));
  assert(OK());
}

template <typename PS>
template <typename T>
Pointset_Powerset<PS>::Pointset_Powerset(const BD_Shape<T>& bds,
                                         Complexity_Class)
  : Base(), space_dim(bds.space_dimension()) {
  Pointset_Powerset& x = *this;
  if (!bds.is_empty())
    x.sequence.push_back(Determinate<PS>(PS(bds)));
  assert(OK());
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const Constraint_System& cs)
  : Base(Determinate<PS>(cs)), space_dim(cs.space_dimension()) {
  assert(OK());
}

template <typename PS>
inline
Pointset_Powerset<PS>::Pointset_Powerset(const Congruence_System& cgs)
  : Base(Determinate<PS>(cgs)), space_dim(cgs.space_dimension()) {
  assert(OK());
}

template <typename PS>
inline Pointset_Powerset<PS>&
Pointset_Powerset<PS>::operator=(const Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;
  x.Base::operator=(y);
  x.space_dim = y.space_dim;
  return x;
}

template <typename PS>
inline void
Pointset_Powerset<PS>::swap(Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;
  x.Base::swap(y);
  std::swap(x.space_dim, y.space_dim);
}

template <typename PS>
template <typename QH>
inline Pointset_Powerset<PS>&
Pointset_Powerset<PS>::operator=(const Pointset_Powerset<QH>& y) {
  Pointset_Powerset& x = *this;
  Pointset_Powerset<PS> pps(y);
  x.swap(pps);
  return x;
}

template <typename PS>
inline void
Pointset_Powerset<PS>::intersection_assign(const Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;
  x.pairwise_apply_assign
    (y, CS::lift_op_assign(std::mem_fun_ref(&PS::intersection_assign)));
}

template <typename PS>
inline bool
Pointset_Powerset<PS>
::intersection_assign_and_minimize(const Pointset_Powerset& y) {
  intersection_assign(y);
  return !is_empty();
}

template <typename PS>
inline void
Pointset_Powerset<PS>::time_elapse_assign(const Pointset_Powerset& y) {
  Pointset_Powerset& x = *this;
  x.pairwise_apply_assign
    (y, CS::lift_op_assign(std::mem_fun_ref(&PS::time_elapse_assign)));
}

template <typename PS>
inline bool
Pointset_Powerset<PS>
::geometrically_covers(const Pointset_Powerset& y) const {
  // This code is only used when PS is an abstraction of NNC_Polyhedron.
  const Pointset_Powerset<NNC_Polyhedron> xx(*this);
  const Pointset_Powerset<NNC_Polyhedron> yy(y);
  return xx.geometrically_covers(yy);
}

template <typename PS>
inline bool
Pointset_Powerset<PS>
::geometrically_equals(const Pointset_Powerset& y) const {
  // This code is only used when PS is an abstraction of NNC_Polyhedron.
  const Pointset_Powerset<NNC_Polyhedron> xx(*this);
  const Pointset_Powerset<NNC_Polyhedron> yy(y);
  return xx.geometrically_covers(yy) && yy.geometrically_covers(xx);
}

template <>
inline bool
Pointset_Powerset<Grid>
::geometrically_equals(const Pointset_Powerset& y) const {
  const Pointset_Powerset& x = *this;
  return x.geometrically_covers(y) && y.geometrically_covers(x);
}

template <>
inline bool
Pointset_Powerset<NNC_Polyhedron>
::geometrically_equals(const Pointset_Powerset& y) const {
  const Pointset_Powerset& x = *this;
  return x.geometrically_covers(y) && y.geometrically_covers(x);
}

template <typename PS>
inline memory_size_type
Pointset_Powerset<PS>::external_memory_in_bytes() const {
  return Base::external_memory_in_bytes();
}

template <typename PS>
inline memory_size_type
Pointset_Powerset<PS>::total_memory_in_bytes() const {
  return sizeof(*this) + external_memory_in_bytes();
}

template <typename PS>
inline int32_t
Pointset_Powerset<PS>::hash_code() const {
  return space_dimension() & 0x7fffffff;
}

template <typename PS>
inline void
Pointset_Powerset<PS>
::difference_assign(const Pointset_Powerset& y) {
  // This code is only used when PS is an abstraction of NNC_Polyhedron.
  Pointset_Powerset<NNC_Polyhedron> nnc_this(*this);
  Pointset_Powerset<NNC_Polyhedron> nnc_y(y);
  nnc_this.difference_assign(nnc_y);
  *this = nnc_this;
}

/*! \relates Pointset_Powerset */
template <typename PS>
inline bool
check_containment(const PS& ph, const Pointset_Powerset<PS>& ps) {
  // This code is only used when PS is an abstraction of NNC_Polyhedron.
  const NNC_Polyhedron pph = NNC_Polyhedron(ph.constraints());
  const Pointset_Powerset<NNC_Polyhedron> pps(ps);
  return check_containment(pph, pps);
}

/*! \relates Pointset_Powerset */
template <>
inline bool
check_containment(const C_Polyhedron& ph,
		  const Pointset_Powerset<C_Polyhedron>& ps) {
  return check_containment(NNC_Polyhedron(ph),
			   Pointset_Powerset<NNC_Polyhedron>(ps));
}

} // namespace Parma_Polyhedra_Library


namespace std {

/*! \relates Parma_Polyhedra_Library::Pointset_Powerset */
template <typename PS>
inline void
swap(Parma_Polyhedra_Library::Pointset_Powerset<PS>& x,
     Parma_Polyhedra_Library::Pointset_Powerset<PS>& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_Pointset_Powerset_inlines_hh)
