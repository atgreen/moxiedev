/* Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is free software; as a special exception the author gives
unlimited permission to copy and/or distribute it, with or without
modifications, as long as this notice is preserved.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. */

#ifndef PPL_Checked_Number_types_hh
#define PPL_Checked_Number_types_hh 1

#include "Coefficient_traits_template.hh"

namespace Parma_Polyhedra_Library {

struct Checked_Number_Default_Policy;
struct Extended_Number_Policy;

template <typename T, typename Policy = Checked_Number_Default_Policy>
class Checked_Number;

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Checked_Number_types_hh)
