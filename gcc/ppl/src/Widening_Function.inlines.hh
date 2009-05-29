/* Widening_Function class implementation: inline functions.
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

#ifndef PPL_Widening_Function_inlines_hh
#define PPL_Widening_Function_inlines_hh 1

#include <cassert>

namespace Parma_Polyhedra_Library {

template <typename PH>
Widening_Function<PH>::Widening_Function(Widening_Method wm)
  : w_method(wm) {
}

template <typename PH>
inline void
Widening_Function<PH>::
operator()(PH& x, const PH& y, unsigned* tp) const {
  (x.*w_method)(y, tp);
}

template <typename PH, typename CS>
Limited_Widening_Function<PH, CS>::
Limited_Widening_Function(Limited_Widening_Method lwm,
			  const CS& cs)
  : lw_method(lwm), limiting_cs(cs) {
}

template <typename PH, typename CS>
inline void
Limited_Widening_Function<PH, CS>::
operator()(PH& x, const PH& y, unsigned* tp) const {
  (x.*lw_method)(y, limiting_cs, tp);
}

/*! \relates Pointset_Powerset */
template <typename PH>
inline Widening_Function<PH>
widen_fun_ref(void (PH::* wm)(const PH&, unsigned*)) {
  return Widening_Function<PH>(wm);
}

/*! \relates Pointset_Powerset */
template <typename PH, typename CS>
inline Limited_Widening_Function<PH, CS>
widen_fun_ref(void (PH::* lwm)(const PH&, const CS&, unsigned*),
	      const CS& cs) {
  return Limited_Widening_Function<PH, CS>(lwm, cs);
}

} // namespace Parma_Polyhedra_Library


#endif // !defined(PPL_Widening_Function_inlines_hh)
