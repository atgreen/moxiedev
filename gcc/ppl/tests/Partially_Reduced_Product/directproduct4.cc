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

// affine_image()
bool
test01() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence((B %= 2) / 14);
  dp.refine_with_constraint(A <= 5);
  dp.refine_with_constraint(B <= 10);

  dp.affine_image(A, B + C);

  Product known_dp(3);
  known_dp.refine_with_congruence((B %= 2) / 14);
  known_dp.refine_with_constraint(A - B - C == 0);
  known_dp.refine_with_constraint(A - C <= 10);
  known_dp.refine_with_constraint(B <= 10);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// affine_preimage()
bool
test02() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_constraint(A - B == 0);
  dp.refine_with_congruence((A %= 0) / 3);

  dp.affine_preimage(A, B);

  Product known_dp(3);
  known_dp.refine_with_congruence((B %= 0) / 3);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

#if !Box_Class
// generalized_affine_image(v, EQUAL, e)
bool
test03() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_congruence(A %= 0);
  dp.refine_with_congruence((A + B %= 0) / 2);
  dp.refine_with_constraint(B >= 0);
  dp.refine_with_constraint(A - B >= 0);

  Linear_Expression le(A+2);

  dp.generalized_affine_image(A, EQUAL, le);

  Product known_dp(3);
  known_dp.refine_with_congruence(A %= 0);
  known_dp.refine_with_congruence((A + B %= 0) / 2);
  known_dp.refine_with_constraint(B >= 0);
  known_dp.refine_with_constraint(A - B >= 2);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_image(v, EQUAL, e, d)
bool
test04() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_congruence(A %= 0);
  dp.refine_with_congruence((A + B %= 0) / 2);
  dp.refine_with_constraint(A >= 3);

  dp.generalized_affine_image(B, EQUAL, A + 1, 2);

  Product known_dp(3);
  known_dp.refine_with_constraint(A - 2*B == -1);
  known_dp.refine_with_congruence(A %= 0);
  known_dp.refine_with_constraint(A >= 3);
  known_dp.refine_with_constraint(B >= 2);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_preimage(v, GREATER_OR_EQUAL, e)
bool
test05() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_constraint(A >= 0);
  dp.refine_with_constraint(A <= 4);
  dp.refine_with_constraint(B <= 5);
  dp.refine_with_constraint(A <= B);
  dp.refine_with_congruence(A %= B);

  dp.generalized_affine_preimage(B, GREATER_OR_EQUAL, A+2);

  Product known_dp(3);
  known_dp.refine_with_constraint(0 <= A);
  known_dp.refine_with_constraint(A <= 3);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_preimage(v, EQUAL, e, d),
bool
test06() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence(A %= 0);
  dp.refine_with_congruence((B %= 0) / 2);

  dp.generalized_affine_preimage(B, EQUAL, A + B, 1);

  Product known_dp(3);
  known_dp.refine_with_congruence((A + B %= 0) / 2);
  known_dp.refine_with_congruence(A %= 0);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_image(lhs, relsym, rhs)
bool
test07() {
  Variable A(0);
  Variable B(1);

  Product dp(3);
  dp.refine_with_congruence(A %= 0);
  dp.refine_with_constraint(B >= 0);
  dp.refine_with_constraint(A - B >= 1);

  dp.generalized_affine_image(Linear_Expression(2), LESS_OR_EQUAL, A + B);

  Product known_dp(3);
  known_dp.refine_with_congruence(A %= 0);
  known_dp.refine_with_constraint(B >= 0);
  known_dp.refine_with_constraint(A - B >= 1);
  known_dp.refine_with_constraint(2 <= A + B);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_image(lhs, EQUAL, rhs),
// refine_with_congruences(cgs)
bool
test08() {
  Variable A(0);
  Variable B(1);

  Congruence_System cs;
  cs.insert((A %= 0) / 1);
  cs.insert((B %= 0) / 2);

  Product dp(2);
  dp.refine_with_congruences(cs);
  dp.refine_with_constraint(A <= 3);

  dp.generalized_affine_image(A + 2*B, EQUAL, A - B);

  Product known_dp(2);
  known_dp.refine_with_congruence((A + 2*B %= 0) / 1);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_preimage(lhs, relsym, rhs), refine_with_constraints(cs)
bool
test09() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(A <= 4);
  cs.insert(B <= 5);
  cs.insert(A <= B);

  Product dp(3);
  dp.refine_with_constraints(cs);
  dp.refine_with_congruence(A %= B);

  dp.generalized_affine_preimage(1*B, GREATER_OR_EQUAL, A+2);

  Product known_dp(3);
  known_dp.refine_with_constraint(0 <= A);
  known_dp.refine_with_constraint(A <= 3);

  bool ok = (dp == known_dp);

  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}

// generalized_affine_preimage(lhs, EQUAL, rhs)
bool
test10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_constraint(A - B == 1);

  dp.generalized_affine_preimage(A - B, EQUAL, 2*A - 2*B);

  Product known_dp(3);
  known_dp.refine_with_congruence((2*A - 2*B %= 1) / 0);

#if BD_Shape_Class || Octagonal_Shape_Class
  #ifdef PH_IS_FIRST
    bool ok = (dp.domain2() == known_dp.domain2()
	       && dp.domain1().is_universe());
  #else
    bool ok = (dp.domain1() == known_dp.domain1()
	       && dp.domain2().is_universe());
  #endif
#else
  bool ok = (dp == known_dp);
#endif


  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp, "*** dp congruences ***");

  return ok;
}
#endif

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
#if !Box_Class
  DO_TEST(test03);
  DO_TEST(test04);
  DO_TEST(test05);
  DO_TEST(test06);
  DO_TEST(test07);
  DO_TEST(test08);
  DO_TEST(test09);
  DO_TEST(test10);
#endif
END_MAIN
