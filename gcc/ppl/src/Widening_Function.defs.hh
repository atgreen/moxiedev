/* Widening_Function class declaration.
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

#ifndef PPL_Widening_Function_defs_hh
#define PPL_Widening_Function_defs_hh 1

#include "Widening_Function.types.hh"
#include "Constraint_System.types.hh"

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Wraps a widening method into a function object.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename PH>
class Parma_Polyhedra_Library::Widening_Function {
public:
  //! The (parametric) type of a widening method.
  typedef void (PH::* Widening_Method)(const PH&, unsigned*);

  //! Explicit unary constructor.
  explicit
  Widening_Function(Widening_Method wm);

  //! Function-application operator.
  /*!
    Computes <CODE>(x.*wm)(y, tp)</CODE>, where \p wm is the widening
    method stored at construction time.
  */
  void operator()(PH& x, const PH& y, unsigned* tp = 0) const;

private:
  //! The widening method.
  Widening_Method w_method;
};


#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Wraps a limited widening method into a function object.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename PH, typename CS>
class Parma_Polyhedra_Library::Limited_Widening_Function {
public:
  //! The (parametric) type of a limited widening method.
  typedef void (PH::* Limited_Widening_Method)(const PH&,
					       const CS&,
					       unsigned*);

  //! Constructor.
  /*!
    \param lwm
    The limited widening method.

    \param cs
    The constraint system limiting the widening.
  */
  Limited_Widening_Function(Limited_Widening_Method lwm,
			    const CS& cs);

  //! Function-application operator.
  /*!
    Computes <CODE>(x.*lwm)(y, cs, tp)</CODE>, where \p lwm and \p cs
    are the limited widening method and the constraint system stored
    at construction time.
  */
  void operator()(PH& x, const PH& y, unsigned* tp = 0) const;

private:
  //! The limited widening method.
  Limited_Widening_Method lw_method;
  //! A constant reference to the constraint system limiting the widening.
  const CS& limiting_cs;
};

namespace Parma_Polyhedra_Library {

//! Wraps a widening method into a function object.
/*!
  \relates Pointset_Powerset

  \param wm
  The widening method.
*/
template <typename PH>
Widening_Function<PH>
widen_fun_ref(void (PH::* wm)(const PH&, unsigned*));

//! Wraps a limited widening method into a function object.
/*!
  \relates Pointset_Powerset

  \param lwm
  The limited widening method.

  \param cs
  The constraint system limiting the widening.
*/
template <typename PH, typename CS>
Limited_Widening_Function<PH, CS>
widen_fun_ref(void (PH::* lwm)(const PH&, const CS&, unsigned*),
	      const CS& cs);

} // namespace Parma_Polyhedra_Library

#include "Widening_Function.inlines.hh"

#endif // !defined(PPL_Widening_Function_defs_hh)
