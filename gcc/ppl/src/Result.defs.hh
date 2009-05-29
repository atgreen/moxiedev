/* Result enum and supporting function declarations.
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

#ifndef PPL_Result_defs_hh
#define PPL_Result_defs_hh 1

namespace Parma_Polyhedra_Library {

//! Possible outcomes of a checked arithmetic computation.
/*! \ingroup PPL_CXX_interface */
enum Result {

  //! \hideinitializer Ordinary result class.
  VC_NORMAL = 0,

  //! \hideinitializer The computed result is inexact and rounded up.
  V_LT = 1,

  //! \hideinitializer The computed result is inexact and rounded down.
  V_GT = 2,

  //! \hideinitializer The computed result is exact.
  V_EQ = 4,

  //! \hideinitializer The computed result is inexact.
  V_NE = V_LT | V_GT,

  //! \hideinitializer The computed result may be inexact and rounded up.
  V_LE = V_EQ | V_LT,

  //! \hideinitializer The computed result may be inexact and rounded down.
  V_GE = V_EQ | V_GT,

  //! \hideinitializer The computed result may be inexact.
  V_LGE = V_LT | V_EQ | V_GT,

  //! \hideinitializer Negative infinity unrepresentable result class.
  VC_MINUS_INFINITY = 16,

  //! \hideinitializer A negative overflow occurred.
  V_NEG_OVERFLOW = VC_MINUS_INFINITY | V_GT,

  //! \hideinitializer Positive infinity unrepresentable result class.
  VC_PLUS_INFINITY = 32,

  //! \hideinitializer A positive overflow occurred.
  V_POS_OVERFLOW = VC_PLUS_INFINITY | V_LT,

  //! \hideinitializer Not a number result class.
  VC_NAN = 48,

  //! \hideinitializer Converting from unknown string.
  V_CVT_STR_UNK = 49,

  //! \hideinitializer Dividing by zero.
  V_DIV_ZERO = 50,

  //! \hideinitializer Adding two infinities having opposite signs.
  V_INF_ADD_INF = 51,

  //! \hideinitializer Dividing two infinities.
  V_INF_DIV_INF = 52,

  //! \hideinitializer Taking the modulus of an infinity.
  V_INF_MOD = 53,

  //! \hideinitializer Multiplying an infinity by zero.
  V_INF_MUL_ZERO = 54,

  //! \hideinitializer Subtracting two infinities having the same sign.
  V_INF_SUB_INF = 55,

  //! \hideinitializer Computing a remainder modulo zero.
  V_MOD_ZERO = 56,

  //! \hideinitializer Taking the square root of a negative number.
  V_SQRT_NEG = 57,

  //! \hideinitializer Unknown result due to intermediate negative overflow.
  V_UNKNOWN_NEG_OVERFLOW = 58,

  //! \hideinitializer Unknown result due to intermediate positive overflow.
  V_UNKNOWN_POS_OVERFLOW = 59,

  //! \hideinitializer Unordered comparison.
  V_UNORD_COMP = 60,

  VC_MASK = 48
};

//! Extracts the class part of \p r (normal, minus/plus infinity or nan).
Result classify(Result r);

//! Returns <CODE>true</CODE> if and only if the class or \p r is not normal.
bool is_special(Result r);

} // namespace Parma_Polyhedra_Library

#include "Result.inlines.hh"

#endif // !defined(PPL_Result_defs_hh)
