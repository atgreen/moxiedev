/* Test Direct_Product<NNC_Polyhedron, Grid>.
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

#define PH_IS_FIRST

// ONE AND ONLY ONE OF THESE MUST BE 1
#define NNC_Poly_Class 1
#define C_Poly_Class 0
#define BD_Shape_Class 0
#define Octagonal_Shape_Class 0
#define Box_Class 0

#if Box_Class
typedef TBox Poly;
#endif

#if Octagonal_Shape_Class
typedef TOctagonal_Shape Poly;
#endif

#if BD_Shape_Class
typedef BD_Shape<mpq_class> Poly;
#endif

#if NNC_Poly_Class
typedef NNC_Polyhedron Poly;
#endif

#if C_Poly_Class
typedef C_Polyhedron Poly;
#endif

#ifdef PH_IS_FIRST
typedef Domain_Product<Poly, Grid>::Direct_Product Product;
#else
typedef Domain_Product<Grid, Poly>::Direct_Product Product;
#endif

namespace {

// is_empty() where both domain objects have points.
bool
test01() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence(A %= 9);
  dp.refine_with_congruence(B + C %= 3);

  bool ok = !dp.is_empty();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_empty() where one domain object is empty.
bool
test02() {
  Variable A(0);

  Product dp(3);

  dp.refine_with_congruence((A %= 0) / 2);
  dp.refine_with_congruence((A %= 1) / 2);

  bool ok = dp.is_empty();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_empty() where both domain objects are empty.
bool
test03() {
  Variable A(0);

  Product dp(3);
  dp.refine_with_constraint(A == 1);
  dp.refine_with_constraint(A == 3);

  bool ok = dp.is_empty();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_universe() where both domain objects are empty.
bool
test04() {
  Product dp(3, EMPTY);

  bool ok = !dp.is_universe();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_universe() where one domain object is universe.
bool
test05() {
  Variable A(0);

  Product dp(3);
  dp.refine_with_congruence((A %= 0) / 2);
  dp.refine_with_congruence((A %= 1) / 2);

  bool ok = !dp.is_universe();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_universe() where both domain objects are universe.
bool
test06() {
  Product dp(3);

  bool ok = dp.is_universe();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_topologically_closed() where the NNC Polyhedron is topologically
// open.
bool
test07() {
  Variable A(0);

  Product dp(3);
  dp.refine_with_constraint(A < 3);
  dp.refine_with_congruence((A %= 0) / 3);

#if NNC_Poly_Class || Box_Class
  bool ok = !dp.is_topologically_closed();
#else
  dp.refine_with_constraint(A <= 3);
  bool ok = dp.is_topologically_closed();
#endif

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_topologically_closed() where the Polyhedron is topologically
// closed.
bool
test08() {
  Variable A(0);

  Product dp(3);
  dp.refine_with_constraint(A <= 3);
  dp.refine_with_congruence((A %= 0) / 3);

  bool ok = dp.is_topologically_closed();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_topologically_closed() where the Polyhedron is topologically
// open and the intersection is closed.
bool
test09() {
  Variable A(0);

  Product dp(3);
  dp.refine_with_congruence((A %= 0) / 4);
  dp.refine_with_constraint(A < 3);

#if NNC_Poly_Class
  bool ok = !dp.is_topologically_closed();
#else
  dp.refine_with_constraint(A <= 2);

  bool ok = dp.is_topologically_closed();
#endif

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_disjoint_from(dp), due to the Polyhedra.
bool
test10() {
  Variable B(1);

  Product dp1(12);
  Product dp2(12);
  dp1.refine_with_constraint(B < 2);
  dp2.refine_with_constraint(B > 3);
  bool ok = dp1.is_disjoint_from(dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok;
}

// is_disjoint_from(dp), due to the Grids.
bool
test11() {
  Variable A(0);

  Product dp1(3);
  dp1.refine_with_congruence((A %= 0) / 7);

  Product dp2(3);
  dp2.refine_with_congruence((A %= 1) / 7);

  bool ok = dp1.is_disjoint_from(dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok;
}

// is_disjoint_from(dp), due to either.
bool
test12() {
  Variable A(0);

  Product dp1(3);
  dp1.refine_with_congruence((A %= 0) / 7);
  Product dp2(3);
  dp2.refine_with_congruence((A %= 1) / 7);
  dp1.refine_with_constraint(A < 3);
  dp2.refine_with_constraint(A > 3);

  bool ok = dp1.is_disjoint_from(dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok;
}

// is_disjoint_from(dp), due to both.
bool
test13() {
  Variable A(0);

  Product dp1(3);
  dp1.refine_with_congruence((A %= 1) / 7);
  Product dp2(3);
  dp2.refine_with_congruence((A %= 1) / 14);
  dp1.refine_with_constraint(A < 6);
  dp2.refine_with_constraint(A > 3);

  bool ok = !dp1.is_disjoint_from(dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok;
}

// is_disjoint_from(dp), due to the intersection of the entire direct
// products (i.e. the dp1 and dp2 polyhedron components intersect, as
// do the grid components).
bool
test14() {
  Variable A(0);
  Variable B(1);

  Product dp1(2);
  dp1.refine_with_constraint(A <= 4);
  dp1.refine_with_constraint(A >= 0);
  dp1.refine_with_constraint(A - B <= 0);
  dp1.refine_with_constraint(A - B >= 2);
  dp1.refine_with_congruence((A %= 0) / 2);
  dp1.refine_with_congruence((A %= 0) / 4);

  Product dp2(2);
  dp2.refine_with_constraint(A <= 4);
  dp2.refine_with_constraint(A <= 0);
  dp2.refine_with_constraint(A + B >= 4);
  dp2.refine_with_constraint(A + B <= 6);
  // Same grid as dp1.
  dp2.refine_with_congruence((A %= 0) / 2);
  dp2.refine_with_congruence((A %= 0) / 4);

#if Box_Class
  bool ok = !dp1.is_disjoint_from(dp2);
#else
  bool ok = dp1.is_disjoint_from(dp2);
#endif

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok;
}

// is_discrete(), due to grid.
bool
test15() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_constraint(A == 1);
  dp.refine_with_constraint(B == 2);
  dp.refine_with_constraint(C <= 3);
  dp.refine_with_congruence((C %= 0) / 3);

  bool ok = dp.is_discrete();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_discrete(), due to polyhedron.
bool
test16() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_constraint(A <= 3);
  dp.refine_with_constraint(A >= 3);
  dp.refine_with_constraint(B == 0);

  bool ok = dp.is_discrete();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_discrete() is false, as the components are not discrete
// although the intersection is discrete..
bool
test17() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_congruence((A - B %= 0) / 0);
  dp.refine_with_constraint(B >= 0);
  dp.refine_with_constraint(B <= 0);

  bool ok = !dp.is_discrete();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_bounded(), due to polyhedron.
bool
test18() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_congruence((A %= 1) / 3);
  dp.refine_with_constraint(A > 1);
  dp.refine_with_constraint(A < 4);
  dp.refine_with_constraint(B > 1);
  dp.refine_with_constraint(B < 4);

  bool ok = dp.is_bounded();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// is_bounded(), due to grid.
bool
test19() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_congruence((A %= 0) / 0);
  dp.refine_with_congruence((B %= 0) / 0);
  dp.refine_with_constraint(B <= 0);

  bool ok = dp.is_bounded();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
  DO_TEST(test05);
  DO_TEST(test06);
  DO_TEST(test07);
  DO_TEST(test08);
  DO_TEST(test09);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST(test17);
  DO_TEST(test18);
  DO_TEST(test19);
END_MAIN
