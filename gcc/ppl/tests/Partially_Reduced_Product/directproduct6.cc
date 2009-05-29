/* Test Grid::congruences().
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
site: http://www.cs.unipr.it/ppl/ .

Tests bounds_from_above(), bounds_from_below(), maximize() and minimize()
for the product domains */

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

// Empty.
bool
test01() {
  Product dp(7, EMPTY);

  bool ok = (dp.bounds_from_above(Linear_Expression(0))
	     && dp.bounds_from_below(Linear_Expression(0)));

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Zero dimension empty.
bool
test02() {
  Product dp(0, EMPTY);

  bool ok = (dp.bounds_from_above(Linear_Expression(3))
	     && dp.bounds_from_below(Linear_Expression(3)));

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Zero dimension universe.
bool
test03() {
  Product dp(0);

  bool ok = (dp.bounds_from_above(Linear_Expression(1))
	     && dp.bounds_from_below(Linear_Expression(1)));

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Point.
bool
test04() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_constraint(A == 1);
  dp.refine_with_constraint(3*B == 2);

  Linear_Expression le = A + B;
  bool ok = dp.bounds_from_above(le)
    && dp.bounds_from_below(le);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// only one component is bounded.
bool
test05() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
#if Box_Class
  dp.refine_with_constraint(A >= 1);
  dp.refine_with_constraint(A <= 1);
  dp.refine_with_constraint(B >= 1);
  dp.refine_with_constraint(B <= 0);
#else
  dp.refine_with_constraint(A - B >= 1);
  dp.refine_with_constraint(A - B <= 1);
#endif
  dp.refine_with_congruence(3*B %= 2);

  Linear_Expression le = A - B;
  bool ok = dp.bounds_from_above(le)
    && dp.bounds_from_below(le);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Line and neither component is bounded.
bool
test06() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_constraint(B == 1);

  Linear_Expression le = 2*A - B;

  bool ok = !dp.bounds_from_above(le)
    && !dp.bounds_from_below(le);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Empty. maximize() and minimize()
bool
test07() {
  Product dp(7, EMPTY);

  Coefficient extr_n;
  Coefficient extr_d;
  bool dummy;

  bool ok = (!dp.maximize(Linear_Expression(0), extr_n, extr_d, dummy));

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Zero dimension empty.
bool
test08() {
  Product dp(0, EMPTY);

  Coefficient extr_n;
  Coefficient extr_d;
  bool dummy;
  Generator pnt(point());

  bool ok = !dp.maximize(Linear_Expression(0), extr_n, extr_d, dummy)
    && !dp.maximize(Linear_Expression(0), extr_n, extr_d, dummy, pnt);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Zero dimension universe.
bool
test09() {
  Product dp(0);

  Coefficient extr_n, extr_d;
  bool dummy;
  Generator pnt(point());

  bool ok = dp.maximize(Linear_Expression(0), extr_n, extr_d, dummy)
    && dp.maximize(Linear_Expression(0), extr_n, extr_d, dummy, pnt);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Point.
bool
test10() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_constraint(A == 1);
  dp.refine_with_constraint(3*B == 2);

  Linear_Expression le = A + B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  Generator known_pnt(point(3*A + 2*B, 3));
  bool max;
  bool min;

  bool ok = dp.maximize(le, max_n, max_d, max)
    && dp.minimize(le, min_n, min_d, min)
    && dp.maximize(le, max_n, max_d, max, pnt_max)
    && dp.minimize(le, min_n, min_d, min, pnt_min);

  ok = ok
    && max && min && max_n == 5 && max_d == 3 && min_n == 5 && min_d == 3
    && pnt_max == known_pnt && pnt_min == known_pnt;

  print_generator(pnt_max, "*** max point ***");
  print_generator(pnt_min, "*** min point ***");

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// only one component is bounded.
bool
test11() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
#if Box_Class
  dp.refine_with_constraint(A >= 2);
  dp.refine_with_constraint(A <= 2);
  dp.refine_with_constraint(B >= 1);
  dp.refine_with_constraint(B <= 1);
#else
  dp.refine_with_constraint(A - B >= 1);
  dp.refine_with_constraint(A - B <= 1);
#endif
  dp.refine_with_congruence(3*B %= 2);
;
  Linear_Expression le = A - B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  bool max;
  bool min;

  bool ok = dp.maximize(le, max_n, max_d, max)
    && dp.minimize(le, min_n, min_d, min)
    && dp.maximize(le, max_n, max_d, max, pnt_max)
    && dp.minimize(le, min_n, min_d, min, pnt_min);

  ok = ok
    && max && min && max_n == 1 && max_d == 1 && min_n == 1 && min_d == 1
    && pnt_max == pnt_min;

  print_generator(pnt_max, "*** maximum point ***");

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Line and neither component is bounded.
bool
test12() {
  Variable A(0);
  Variable B(1);

  Product dp(2);
  dp.refine_with_constraint(B == 1);

  Linear_Expression le = 2*A - B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  bool max;
  bool min;

  bool ok = !dp.maximize(le, max_n, max_d, max)
    && !dp.minimize(le, min_n, min_d, min)
    && !dp.maximize(le, max_n, max_d, max, pnt_max)
    && !dp.minimize(le, min_n, min_d, min, pnt_min);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// only one component is strictly bounded.
bool
test13() {
  Variable A(0);
  Variable B(1);

  Product dp(2);

#if NNC_Poly_Class
  dp.refine_with_constraint(A - B > 0);
  dp.refine_with_constraint(A - B < 1);
#else
#if !Box_Class
  dp.refine_with_constraint(A - B >= 0);
  dp.refine_with_constraint(A - B <= 1);
#else
  dp.refine_with_constraint(A >= 2);
  dp.refine_with_constraint(A <= 2);
  dp.refine_with_constraint(B <= 2);
  dp.refine_with_constraint(B >= 1);
#endif
#endif
  dp.refine_with_congruence(3*B %= 2);

  Linear_Expression le = A - B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());

  bool max;
  bool min;

  bool ok = dp.maximize(le, max_n, max_d, max)
    && dp.minimize(le, min_n, min_d, min)
    && dp.maximize(le, max_n, max_d, max, pnt_max)
    && dp.minimize(le, min_n, min_d, min, pnt_min);

#if NNC_Poly_Class
  ok = ok && !max && !min;
#else
  ok = ok && max && min;
#endif

  ok = ok && max_n == 1 && max_d == 1 && min_n == 0 && min_d == 1;

  print_generator(pnt_max, "*** maximum point ***");

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

#if !Box_Class
// Non-empty product. bounded_affine_image/3
bool
test14() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence((A ==  0) / 0);
  dp.refine_with_congruence((B ==  0) / 0);
  dp.refine_with_congruence((C == -2) / 0);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  dp.bounded_affine_image(A, 7-B, B+3);

  Product known_dp(3);
  known_dp.refine_with_constraint(C == -2);
  known_dp.refine_with_constraint(B == 0);
  known_dp.refine_with_constraint(A <= 3);
  known_dp.refine_with_constraint(A - B >= 7);

  bool ok = (dp == known_dp);

  print_congruences(dp,
     "*** dp.bounded_affine_image(A, 7-B, B+3) congruences ***");
  print_constraints(dp,
     "*** dp.bounded_affine_image(A, 7-B, B+3) constraints ***");

  return ok;
}

// Empty product component. bounded_affine_image/3
bool
test15() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_constraint(A ==  0);
  dp.refine_with_constraint(A ==  1);
  dp.refine_with_constraint(C == -2);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  dp.bounded_affine_image(A, 7-B, B+3);

  Product known_dp(3, EMPTY);

  bool ok = (dp == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Non-empty product. bounded_affine_preimage/3
bool
test16() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_congruence((A ==  0) / 0);
  dp.refine_with_congruence((B ==  0) / 0);
  dp.refine_with_congruence((C == -2) / 0);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  dp.bounded_affine_preimage(A, 7-B, B+3);

  Constraint_System cs;
  cs.insert(C == -2);
  cs.insert(B == 0);
  cs.insert(C >= 3);

  Product known_dp(3);
  known_dp.refine_with_constraints(cs);

  bool ok = (dp == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}

// Empty product. bounded_affine_preimage/3
bool
test17() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Product dp(3);
  dp.refine_with_constraint(A ==  0);
  dp.refine_with_constraint(A ==  1);
  dp.refine_with_constraint(C == -2);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  dp.bounded_affine_preimage(A, 7-B, B+3);

  Product known_dp(3, EMPTY);

  bool ok = (dp == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");

  return ok;
}
#endif

typedef Domain_Product<TBox, Grid>::Direct_Product TBox_Grid;
typedef Domain_Product<Grid, TBox>::Direct_Product Grid_TBox;
typedef Domain_Product<NNC_Polyhedron, Grid>::Direct_Product NNCPoly_Grid;

// TBox_Grid(nnc_polyhedron, POLYNOMIAL_COMPLEXITY).
bool
test18() {
  Variable x(0);
  Variable y(1);

  C_Polyhedron ph(2);
  ph.refine_with_constraint(3*x + y >= 2);
  ph.refine_with_constraint(x <= 4);
  ph.refine_with_constraint(y <= 4);

  TBox_Grid pdp(ph, POLYNOMIAL_COMPLEXITY);

  TBox_Grid ndp(ph);

  TBox_Grid known_ndp(2);
  known_ndp.refine_with_constraint(3*x >= -2);
  known_ndp.refine_with_constraint(x <= 4);
  known_ndp.refine_with_constraint(y >= -10);
  known_ndp.refine_with_constraint(y <= 4);

  TBox_Grid known_pdp(2);
  known_pdp.refine_with_constraint(x <= 4);
  known_pdp.refine_with_constraint(y <= 4);

  bool ok = (ndp == known_ndp && pdp == known_pdp && pdp.contains(ndp));

  print_constraints(ph, "*** ph ***");
  print_constraints(ndp, "*** ndp ***");
  print_congruences(ndp, "*** ndp ***");
  print_constraints(pdp, "*** pdp ***");
  print_congruences(pdp, "*** pdp ***");

  return ok;
}

// Copy constructor.
bool
test19() {
  Variable A(0);

  const Constraint_System cs(A >= 0);

  NNCPoly_Grid src(1);
  src.refine_with_constraints(cs);

  NNCPoly_Grid dp(src, POLYNOMIAL_COMPLEXITY);

  NNCPoly_Grid dp1(src);

  NNCPoly_Grid known_dp(1);
  known_dp.refine_with_constraint(A >= 0);

  bool ok = (dp == known_dp && dp1 == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");

  return ok && dp.OK();
}

// Constructing an NNCPoly_Grid from a TBox_Grid.
bool
test20() {
  Variable A(0);

  const Constraint_System cs(A >= 0);
  const Congruence_System cgs(A %= 0);

  TBox_Grid src(1);
  src.refine_with_constraints(cs);
  src.refine_with_congruences(cgs);

  NNCPoly_Grid dp(src, POLYNOMIAL_COMPLEXITY);

  NNCPoly_Grid dp1(src);

  NNCPoly_Grid known_dp(1);
  known_dp.refine_with_constraint(A >= 0);
  known_dp.refine_with_congruence(A %= 0);

  bool ok = (dp == known_dp && dp1 == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");

  return ok && dp.OK();
}

// Constructing an NNCPoly_Grid from a Grid_TBox.
bool
test21() {
  Variable A(0);

  const Constraint_System cs(A >= 0);
  const Congruence_System cgs(A %= 0);

  Grid_TBox src(1);
  src.refine_with_constraints(cs);
  src.refine_with_congruences(cgs);

  NNCPoly_Grid dp(src, POLYNOMIAL_COMPLEXITY);

  NNCPoly_Grid dp1(src);

  NNCPoly_Grid known_dp(1);
  known_dp.refine_with_constraint(A >= 0);
  known_dp.refine_with_congruence(A %= 0);

  bool ok = (dp == known_dp && dp1 == known_dp);

  print_congruences(dp, "*** dp congruences ***");
  print_constraints(dp, "*** dp constraints ***");
  print_congruences(dp1, "*** dp1 congruences ***");
  print_constraints(dp1, "*** dp1 constraints ***");

  return ok && dp.OK();
}

// Attempt to construct a product with too many dimensions.
bool
test22() {
  try {
    // This is an invalid use of the constructor of a product:
    // it is illegal to (try to) build a product with a dimensions
    // greater than max_space_dimension().
    NNCPoly_Grid pg(NNCPoly_Grid::max_space_dimension() + 1);

    // It is an error if the exception is not thrown.
  }
  catch (std::length_error& e) {
    nout << "length_error: " << e.what() << endl << endl;
    return true;
  }
  catch (...) {
    // It is an error if the wrong exception is thrown.
  }
  return false;
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
#if !Box_Class
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST(test17);
#endif
  DO_TEST(test18);
  DO_TEST(test19);
  DO_TEST(test20);
  DO_TEST(test21);
  DO_TEST(test22);
END_MAIN
