/* Checked_Number class implementation
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

#include <ppl-config.h>
#include "Checked_Number.defs.hh"

namespace Parma_Polyhedra_Library {

void
throw_result_exception(Result r) {
  switch (r) {
  case V_LT:
    throw std::logic_error("Exact result is less than computed one.");
  case V_LE:
    throw std::logic_error("Exact result is less than or equal to "
			   "computed one.");
  case V_GT:
    throw std::logic_error("Exact result is greater than computed one.");
  case V_GE:
    throw std::logic_error("Exact result is greater than or equal to "
			   "computed one.");
  case V_NE:
    throw std::logic_error("Exact result is less than or greater than "
			   "computed one.");
  case V_LGE:
    throw std::logic_error("Exact result is less than, greater than or "
			   "equal to computed one.");
  case VC_MINUS_INFINITY:
    throw std::overflow_error("Minus infinity.");
  case V_NEG_OVERFLOW:
    throw std::overflow_error("Negative overflow.");
  case V_UNKNOWN_NEG_OVERFLOW:
    throw std::overflow_error("Unknown result due to negative overflow.");
  case VC_PLUS_INFINITY:
    throw std::overflow_error("Plus infinity.");
  case V_POS_OVERFLOW:
    throw std::overflow_error("Positive overflow.");
  case V_UNKNOWN_POS_OVERFLOW:
    throw std::overflow_error("Unknown result due to positive overflow.");
  case V_CVT_STR_UNK:
    throw std::domain_error("Invalid numeric string.");
  case V_DIV_ZERO:
    throw std::domain_error("Division by zero.");
  case V_INF_ADD_INF:
    throw std::domain_error("Infinities addition.");
  case V_INF_DIV_INF:
    throw std::domain_error("Infinities division.");
  case V_INF_MOD:
    throw std::domain_error("Remainder of division of infinity.");
  case V_INF_MUL_ZERO:
    throw std::domain_error("Multiplication of infinity and zero.");
  case V_INF_SUB_INF:
    throw std::domain_error("Subtraction of infinities.");
  case V_MOD_ZERO:
    throw std::domain_error("Remainder of division by zero.");
  case V_SQRT_NEG:
    throw std::domain_error("Square root of negative number.");
  case V_UNORD_COMP:
    throw std::domain_error("Unordered comparison.");
  default:
    throw std::logic_error("Unexpected result.");
  }
}

} // namespace Parma_Polyhedra_Library

