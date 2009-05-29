/* Test Partially_Reduced_Product<>.
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

#include "ppl_test.hh"

using namespace Parma_Polyhedra_Library::IO_Operators;

// #define PH_IS_NNC
// #define PH_IS_FIRST

// #define PH_IS_BOX

#ifdef PH_IS_NNC
#define PH_IS_BOX_OR_NNC
#endif
#ifdef PH_IS_BOX
#define PH_IS_BOX_OR_NNC
#endif

#ifdef PH_IS_BOX
typedef TBox Poly;
#else
#ifdef PH_IS_NNC
typedef NNC_Polyhedron Poly;
#else
typedef C_Polyhedron Poly;
#endif
#endif

#ifdef PH_IS_FIRST
typedef Domain_Product<Poly, Grid>::Direct_Product DProduct;
typedef Domain_Product<Poly, Grid>::Smash_Product SProduct;
typedef Domain_Product<Poly, Grid>::Constraints_Product CProduct;
#else
typedef Domain_Product<Grid, Poly>::Direct_Product DProduct;
typedef Domain_Product<Grid, Poly>::Smash_Product SProduct;
typedef Domain_Product<Grid, Poly>::Constraints_Product CProduct;
#endif

namespace {

// remove_higher_dimensions()
// The initial product is empty with 1 space dimension
// and this dimension is removed.
bool
test01() {
  Variable A(0);

  SProduct sp1(1);
  CProduct cp1(1);
  Constraint_System cs;
  cs.insert(A >= 1);
  cs.insert(A <= 0);
  sp1.refine_with_constraints(cs);
  cp1.refine_with_constraints(cs);
  SProduct sp2(sp1);
  CProduct cp2(cp1);

  bool ok1s = (sp1 == sp2 && sp2.is_empty());
  if (!ok1s || !sp1.OK()) {
    print_congruences(sp1, "*** after == check: sp1 congruences ***");
    print_constraints(sp1, "*** after == check: sp1 constraints ***");
    return false;
  }

  bool ok1 = (cp1 == cp2 && cp2.is_empty());
  if (!ok1 || !cp1.OK()) {
    print_congruences(cp1, "*** after == check: cp1 congruences ***");
    print_constraints(cp1, "*** after == check: cp1 constraints ***");
    return false;
  }

  sp1.remove_higher_space_dimensions(0);

  if (!sp1.OK()) {
    print_congruences(sp1, "*** remove all dimensions: sp1 congruences ***");
    print_constraints(sp1, "*** remove all dimensions: sp1 constraints ***");
    return false;
  }

  cp1.remove_higher_space_dimensions(0);

  bool ok = cp1.OK();

  print_congruences(cp1, "*** remove all dimensions: cp1 congruences ***");
  print_constraints(cp1, "*** remove all dimensions: cp1 constraints ***");

  return ok;
}

// upper_bound_assign(cp2)
// The first product is empty and the second a single point in 1D
bool
test02() {
  Variable A(0);

  SProduct sp1(1);
  CProduct cp1(1);
  Constraint_System cs1;
  cs1.insert(A >= 1);
  cs1.insert(A <= 0);
  sp1.refine_with_constraints(cs1);
  cp1.refine_with_constraints(cs1);

  SProduct sp2(1);
  CProduct cp2(1);
  Constraint_System cs2;
  cs2.insert(A == 1);
  sp2.refine_with_constraints(cs2);
  cp2.refine_with_constraints(cs2);

  SProduct sp1_copy(sp1);
  CProduct cp1_copy(cp1);

  sp1.upper_bound_assign(sp2);
  cp1.upper_bound_assign(cp2);

  if (!sp1.OK()) {
    print_congruences(sp1, "*** after ok check: sp1 congruences ***");
    print_constraints(sp1, "*** after ok check: sp1 constraints ***");
    return false;
  }

  bool ok = cp1.OK();

  print_congruences(cp1, "*** after OK() check: cp1 congruences ***");
  print_constraints(cp1, "*** after OK() check: cp1 constraints ***");

  return ok;
}

// refine_with_constraints() and refine_with_congruences()
bool
test03() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  CProduct cp(2);
  Constraint_System cs;
  cs.insert(A >= 1);
  cs.insert(A <= 6);
  sp.refine_with_constraints(cs);
  cp.refine_with_constraints(cs);

  SProduct known_sp(sp);
  CProduct known_cp(cp);

  Constraint_System cs1;
  cs1.insert(A > 2);
  cs1.insert(B >= 2);
  Congruence_System cgs1;
  cgs1.insert((B %= 2) / 4);
  cgs1.insert((A + B %= 6) / 0);
  sp.refine_with_constraint(A > 2);
  sp.refine_with_constraint(B >= 2);
  sp.refine_with_congruence((B %= 2) / 4);
  sp.refine_with_congruence((A + B %= 6) / 0);
  cp.refine_with_constraint(A > 2);
  cp.refine_with_constraint(B >= 2);
  cp.refine_with_congruence((B %= 2) / 4);
  cp.refine_with_congruence((A + B %= 6) / 0);

  bool ok = sp.OK() && cp.OK();

#ifdef PH_IS_BOX_OR_NNC
  known_sp.refine_with_constraint(A > 2);
#else
  known_sp.refine_with_constraint(A >= 2);
#endif
  known_sp.refine_with_constraint(B >= 2);
  known_sp.refine_with_congruence((B %= 2) / 4);
  known_sp.refine_with_congruence((A + B %= 6) / 0);
#ifdef PH_IS_BOX_OR_NNC
  known_cp.refine_with_constraint(A > 2);
#else
  known_cp.refine_with_constraint(A >= 2);
#endif
  known_cp.refine_with_constraint(B >= 2);
  known_cp.refine_with_congruence((B %= 2) / 4);
  known_cp.refine_with_congruence((A + B %= 6) / 0);

  ok = ok && sp == known_sp && cp == known_cp;

  print_congruences(sp, "*** after ok check: sp congruences ***");
  print_constraints(sp, "*** after ok check: sp constraints ***");

  print_congruences(cp, "*** after ok check: cp congruences ***");
  print_constraints(cp, "*** after ok check: cp constraints ***");

  return ok;
}

// refine_with_constraints() and refine_with_congruences()
bool
test04() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  CProduct cp(2);
  Constraint_System cs;
  cs.insert(A >= 1);
  cs.insert(A <= 6);
  sp.refine_with_constraints(cs);
  cp.refine_with_constraints(cs);

  SProduct known_sp(sp);
  CProduct known_cp(cp);

  Constraint_System cs1;
  cs1.insert(A > 2);
  cs1.insert(B >= 2);
  Congruence_System cgs1;
  cgs1.insert((B %= 2) / 4);
  cgs1.insert((A + B %= 6) / 0);
  sp.refine_with_constraints(cs1);
  sp.refine_with_congruences(cgs1);
  cp.refine_with_constraints(cs1);
  cp.refine_with_congruences(cgs1);

  bool ok = sp.OK() && cp.OK();

#ifdef PH_IS_BOX_OR_NNC
  known_sp.refine_with_constraint(A > 2);
#else
  known_sp.refine_with_constraint(A >= 2);
#endif
  known_sp.refine_with_constraint(B >= 2);
  known_sp.refine_with_congruence((B %= 2) / 4);
  known_sp.refine_with_congruence((A + B %= 6) / 0);
#ifdef PH_IS_BOX_OR_NNC
  known_cp.refine_with_constraint(A > 2);
#else
  known_cp.refine_with_constraint(A >= 2);
#endif
  known_cp.refine_with_constraint(B >= 2);
  known_cp.refine_with_congruence((B %= 2) / 4);
  known_cp.refine_with_congruence((A + B %= 6) / 0);

  ok = ok && sp == known_sp && cp == known_cp;

  print_congruences(sp, "*** after ok check: sp congruences ***");
  print_constraints(sp, "*** after ok check: sp constraints ***");

  print_congruences(cp, "*** after ok check: cp congruences ***");
  print_constraints(cp, "*** after ok check: cp constraints ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
END_MAIN
