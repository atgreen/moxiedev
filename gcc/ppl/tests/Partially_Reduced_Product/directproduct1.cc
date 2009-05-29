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

// #define PH_IS_FIRST

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
typedef TBD_Shape Poly;
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

// Universe product in 0 dimensions
bool
test01() {
  Product dp1;

  Product dp2(0, UNIVERSE);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Empty product(dims, type)
bool
test02() {
  Product dp1(3);

  Product dp2(3, EMPTY);

  bool ok = (dp1 != dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(cgs), refine_with_congruence(cg)
bool
test03() {
  Variable A(0);

  const Congruence_System cgs((A == 0) / 0);

  Product dp1(cgs);

  Product dp2(1);
  dp2.refine_with_congruence((A == 0) / 0);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_constraints(dp1.minimized_constraints(), "*** dp1 minimized_constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(cgs), domain1(), domain2()
bool
test04() {
  Variable A(0);

  Congruence_System cgs((A %= 0) / 4);

  Product dp(1);
  dp.refine_with_congruence((A %= 0) / 4);

  Poly known_ph(1);

  Grid known_gr(1);
  known_gr.refine_with_congruence((A %= 0) / 4);

#ifdef PH_IS_FIRST
  bool ok = (dp.domain2() == known_gr
	     && dp.domain1() == known_ph);
#else
  bool ok = (dp.domain1() == known_gr
	     && dp.domain2() == known_ph);
#endif

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// Product(cs), refine_with_constraint(c)
bool
test05() {
  Variable A(0);

  const Constraint_System cs(A == 0);

  Product dp1(cs);

  Product dp2(1);
  dp2.refine_with_constraint(static_cast<const Constraint>(A == 0));

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(cs), refine_with_congruence(c)
bool
test06() {
  Variable A(0);

  Constraint_System cs(A == 9);

  Product dp1(cs);

  Product dp2(1);
  dp2.refine_with_constraint(A == 9);

  Grid known_gr(1);
  known_gr.refine_with_congruence((A %= 9) / 0);

#ifdef PH_IS_FIRST
  bool ok = (dp1 == dp2 && dp1.domain2() == known_gr);
#else
  bool ok = (dp1 == dp2 && dp1.domain1() == known_gr);
#endif

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(bounding_box)
bool
test07() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.refine_with_constraint(3*B >= 2);
  box.refine_with_constraint(A >= 2);
  box.refine_with_constraint(A <= 2);

  Product dp(box);

  Product known_dp(2);
  known_dp.refine_with_constraint(3*B >= 2);
  known_dp.refine_with_constraint(A == 2);

  bool ok = (dp == known_dp) && dp.OK();

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// operator=
bool
test08() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs(A + B >= 9);

  Product dp1(2);
  dp1.refine_with_constraints(cs);
  dp1.refine_with_congruence((A %= 9) / 19);

  Product dp2 = dp1;

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Copy constructor.
bool
test09() {
  Variable A(0);
  Variable C(2);

  Product dp1(3);
  dp1.refine_with_constraint(A - C == 0);

  Product dp2(dp1);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// congruences()
bool
test10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence(A %= 9);
  dp.refine_with_congruence(B + C %= 3);

  Congruence_System cgs;
  cgs.insert(B + C %= 0);
  cgs.insert(A %= 0);

  Grid known_gr(cgs);

  Grid gr(dp.congruences());

  bool ok = gr == known_gr;

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// minimized_congruences()
bool
test11() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence(B + C %= 3);
  dp.refine_with_constraint(A >= 9);
  dp.refine_with_constraint(A <= 9);

  Congruence_System cgs;
  cgs.insert(B + C %= 3);
  cgs.insert(A == 9);

  Grid known_gr(cgs);

  Grid gr(dp.minimized_congruences());

  bool ok = gr == known_gr;

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// constraints()
bool
test12() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence((B + C %= 3) / 0);
  dp.refine_with_constraint(A > 9);
  dp.refine_with_constraint(A <= 11);

  Poly ph(dp.space_dimension());
  ph.refine_with_constraints(dp.constraints());

  Poly known_ph(dp.space_dimension());
  known_ph.refine_with_constraint(B + C == 3);
  known_ph.refine_with_constraint(A <= 11);
  known_ph.refine_with_constraint(A > 9);

  bool ok = (ph == known_ph);

  print_constraints(ph, "*** ph ***");

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// minimized_constraints()
bool
test13() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence((B + C %= 3) / 0);
  dp.refine_with_constraint(A > 9);
  dp.refine_with_constraint(A <= 11);

  Poly ph(dp.space_dimension());
  ph.refine_with_constraints(dp.minimized_constraints());

  Poly known_ph(dp.space_dimension());
  known_ph.refine_with_constraint(B + C == 3);
  known_ph.refine_with_constraint(A > 9);
  known_ph.refine_with_constraint(A <= 11);

  bool ok = (ph == known_ph);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok && dp.OK();
}

// Product(c_polyhedron).
bool
test14() {
  Variable A(0);

  const Constraint_System cs(A == 0);

  C_Polyhedron ph(cs);

  Product dp1(ph);

  Product dp2(1);
  dp2.refine_with_congruence((A %= 0) / 0);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(nnc_polyhedron).
bool
test15() {
  Variable A(0);

  const Constraint_System cs(A > 0);

  NNC_Polyhedron ph(cs);

  Product dp1(ph);

  Product dp2(1);
  dp2.refine_with_constraint(A > 0);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(grid).
bool
test16() {
  Variable A(0);

  const Congruence_System cgs(A %= 0);

  Grid gr(cgs);

  Product dp1(gr);

  Product dp2(1);
  dp2.refine_with_congruence((A %= 0) / 1);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(box).
bool
test17() {
  Variable A(0);

  const Constraint_System cs(A > 0);

  TBox box(cs);

  Product dp1(box);

  Product dp2(1);
  dp2.refine_with_constraint(A > 0);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(bds).
bool
test18() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs(A >= 0);
  cs.insert(2*A - 2*B >= 5);

  TBD_Shape bd(cs);

  Product dp1(bd);

  Product dp2(2);
  dp2.refine_with_constraint(A >= 0);
  dp2.refine_with_constraint(2*A - 2*B >= 5);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
}

// Product(os).
bool
test19() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs(A >= 0);
  cs.insert(2*A + 2*B >= 5);

  TOctagonal_Shape os(cs);

  Product dp1(os);

  Product dp2(2);
  dp2.refine_with_constraint(A >= 0);
  dp2.refine_with_constraint(2*A + 2*B >= 5);

  bool ok = (dp1 == dp2);

  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");
  print_congruences(dp2, "*** dp2 congruences ***");
  print_constraints(dp2, "*** dp2 constraints ***");

  return ok && dp1.OK() && dp2.OK();
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
  DO_TEST(test19);
END_MAIN
