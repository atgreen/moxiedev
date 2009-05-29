/* Implementation of some math utility functions: inline functions.
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

#ifndef PPL_math_utilities_inlines_hh
#define PPL_math_utilities_inlines_hh 1

#include "Coefficient.defs.hh"
#include <limits>
#include <cassert>

namespace Parma_Polyhedra_Library {

inline void
normalize2(Coefficient_traits::const_reference x,
	   Coefficient_traits::const_reference y,
	   Coefficient& nx, Coefficient& ny) {
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  gcd_assign(gcd, x, y);
  exact_div_assign(nx, x, gcd);
  exact_div_assign(ny, y, gcd);
}

template <typename T>
inline T
low_bits_mask(const unsigned n) {
  assert(n < unsigned(std::numeric_limits<T>::digits));
  return n == 0 ? 0 : ~(~(T(0u)) << n);
}

template <typename T, typename Policy>
inline void
numer_denom(const Checked_Number<T, Policy>& from,
	    Coefficient& num, Coefficient& den) {
  assert(!is_not_a_number(from)
	 && !is_minus_infinity(from)
	 && !is_plus_infinity(from));
  PPL_DIRTY_TEMP0(mpq_class, q);
  assign_r(q, from, ROUND_NOT_NEEDED);
  num = q.get_num();
  den = q.get_den();
}

template <typename T, typename Policy>
inline void
div_round_up(Checked_Number<T, Policy>& to,
	     Coefficient_traits::const_reference x,
	     Coefficient_traits::const_reference y) {
  PPL_DIRTY_TEMP0(mpq_class, qx);
  PPL_DIRTY_TEMP0(mpq_class, qy);
  // Note: this code assumes that a Coefficient is always convertible
  // to an mpq_class without loss of precision.
  assign_r(qx, x, ROUND_NOT_NEEDED);
  assign_r(qy, y, ROUND_NOT_NEEDED);
  div_assign_r(qx, qx, qy, ROUND_NOT_NEEDED);
  assign_r(to, qx, ROUND_UP);
}

template <typename N>
inline void
min_assign(N& x, const N& y) {
  if (x > y)
    x = y;
}

template <typename N>
inline void
max_assign(N& x, const N& y) {
  if (x < y)
    x = y;
}

template <typename T, typename Policy>
inline bool
is_even(const Checked_Number<T, Policy>& x) {
  Checked_Number<T, Policy> half_x;
  return div2exp_assign_r(half_x, x, 1, ROUND_DIRECT) == V_EQ
    && is_integer(half_x);
}

template <typename T, typename Policy>
inline bool
is_additive_inverse(const Checked_Number<T, Policy>& x,
		    const Checked_Number<T, Policy>& y) {
  Checked_Number<T, Policy> negated_x;
  return neg_assign_r(negated_x, x, ROUND_DIRECT) == V_EQ
    && negated_x == y;
}

inline bool
is_canonical(const mpq_class& x) {
  if (x.get_den() <= 0)
    return false;
  PPL_DIRTY_TEMP0(mpq_class, temp);
  temp = x;
  temp.canonicalize();
  return temp.get_num() == x.get_num();
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_math_utilities_inlines_hh)
