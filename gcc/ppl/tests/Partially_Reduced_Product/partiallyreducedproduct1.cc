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

#define PH_IS_NNC
// #define PH_IS_FIRST

#ifdef PH_IS_NNC
typedef NNC_Polyhedron Poly;
#else
typedef C_Polyhedron Poly;
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

// Product(dims, type); == and !=
bool
test01() {
  Variable A(0);

  SProduct sp1(3);
  SProduct sp2(3, EMPTY);
  CProduct cp1(3);
  CProduct cp2(3);
  cp2.refine_with_constraint(A == 0);

  bool ok = (sp1 != sp2 && cp1 != cp2);

  if (!ok)
    return false;

  sp1.refine_with_congruence((A %= 0) / 4);
  sp1.refine_with_congruence((A %= 1) / 4);

  cp1.refine_with_constraint(A >= 0);
  cp1.refine_with_constraint(A <= 0);

  ok = (sp1 == sp2 && cp1 == cp2);

  ok = ok && sp1.OK() && sp2.OK() && cp1.OK() && cp2.OK();

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  return ok;
}

// operator=
bool
test02() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs(A + B <= 9);

  SProduct sp1(2);
  sp1.refine_with_congruence((A %= 9) / 19);
  sp1.refine_with_congruence((A %= 8) / 19);
  SProduct sp2 = sp1;
  CProduct cp1(2);
  cp1.refine_with_constraints(cs);
  cp1.refine_with_constraint(A >= 9);
  cp1.refine_with_constraint(A <= 9);
  CProduct cp2 = cp1;

  bool ok =  (sp1 == sp2 && cp1 == cp2);

  ok = ok && sp1.OK() && sp2.OK() && cp1.OK() && cp2.OK();

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  return ok;
}

// Copy constructor.
bool
test03() {
  Variable A(0);
  Variable B(2);

  Constraint_System cs(A - B == 0);

  SProduct sp1(cs);
  sp1.refine_with_congruence((A %= 9) / 19);
  sp1.refine_with_congruence((A %= 8) / 19);
  SProduct sp2(sp1);
  CProduct cp1(cs);
  cp1.refine_with_constraint(A >= 9);
  cp1.refine_with_constraint(A <= 9);
  CProduct cp2(cp1);

  bool ok =  (sp1 == sp2 && cp1 == cp2);

  ok = ok && sp1.OK() && sp2.OK() && cp1.OK() && cp2.OK();

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  return ok;
}

// affine_dimension()
bool
test04() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Constraint_System cs;
  cs.insert(A - C <= 9);
  cs.insert(A - C >= 9);
  cs.insert(B == 2);

  SProduct sp(3);
  sp.refine_with_constraints(cs);
  CProduct cp(3);
  cp.refine_with_constraints(cs);

  bool ok = (sp.affine_dimension() == 1
             && cp.affine_dimension() == 1
	     && sp.domain1().affine_dimension() == 2
	     && sp.domain2().affine_dimension() == 1
	     && cp.domain1().affine_dimension() == 1
	     && cp.domain2().affine_dimension() == 1);

  ok = ok && sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// congruences()
bool
test05() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Constraint_System cs1;
  cs1.insert(A - C <= 8);
  cs1.insert(A - C >= 9);
  Constraint_System cs2;
  cs2.insert(A - C <= 9);
  cs2.insert(A - C >= 9);

  SProduct sp(3);
  sp.refine_with_constraints(cs1);
  CProduct cp(3);
  cp.refine_with_constraints(cs2);

  DProduct known_dp1(3, EMPTY);
  DProduct known_dp2(3);
  known_dp2.refine_with_constraint(A - C == 9);

  DProduct dp1(3);
  dp1.refine_with_congruences(sp.congruences());
  DProduct dp2(3);
  dp2.refine_with_congruences(cp.congruences());

  bool ok = (dp1 == known_dp1 && dp2 == known_dp2);

  ok = ok && sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// minimized_congruences()
bool
test06() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Constraint_System cs1;
  cs1.insert(A - C <= 8);
  cs1.insert(A - C >= 9);
  Constraint_System cs2;
  cs2.insert(A - C <= 9);
  cs2.insert(A - C >= 9);


  SProduct sp(3);
  sp.refine_with_constraints(cs1);
  CProduct cp(3);
  cp.refine_with_constraints(cs2);

  DProduct known_dp1(3, EMPTY);
  DProduct known_dp2(3);
  known_dp2.refine_with_constraint(A - C == 9);

  DProduct dp1(3);
  dp1.refine_with_congruences(sp.minimized_congruences());
  DProduct dp2(3);
  dp2.refine_with_congruences(cp.minimized_congruences());

  bool ok = (dp1 == known_dp1 && dp2 == known_dp2);

  ok = ok && sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// constraints()
bool
test07() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

#ifdef PH_IS_NNC
  Constraint c(A > 9);
#else
  Constraint c(A >= 9);
#endif

  SProduct sp(3);
  sp.refine_with_congruence((B + C %= 3) / 0);
  sp.refine_with_constraint(c);
  sp.refine_with_constraint(A <= 8);
  CProduct cp(3);
  cp.refine_with_congruence((B + C %= 3) / 0);
  cp.refine_with_constraint(c);
  cp.refine_with_constraint(B <= 11);
  cp.refine_with_constraint(B >= 11);

  DProduct dp1(sp.space_dimension());
  DProduct dp2(cp.space_dimension());

  dp1.refine_with_constraints(sp.constraints());
  dp2.refine_with_constraints(cp.constraints());

  DProduct known_dp1(sp.space_dimension(), EMPTY);
  DProduct known_dp2(sp.space_dimension());
  known_dp2.refine_with_constraint(C == -8);
  known_dp2.refine_with_constraint(c);
  known_dp2.refine_with_constraint(B == 11);

  bool ok = (dp1 == known_dp1 && dp2 == known_dp2);

  ok = ok && sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// minimized_constraints()
bool
test08() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

#ifdef PH_IS_NNC
  Constraint c(A > 9);
#else
  Constraint c(A >= 9);
#endif

  SProduct sp(3);
  sp.refine_with_congruence((B + C %= 3) / 0);
  sp.refine_with_constraint(c);
  sp.refine_with_constraint(A <= 8);
  CProduct cp(3);
  cp.refine_with_congruence((B + C %= 3) / 0);
  cp.refine_with_constraint(c);
  cp.refine_with_constraint(B <= 11);
  cp.refine_with_constraint(B >= 11);

  DProduct dp1(sp.space_dimension());
  DProduct dp2(cp.space_dimension());

  dp1.refine_with_constraints(sp.minimized_constraints());
  dp2.refine_with_constraints(cp.minimized_constraints());

  DProduct known_dp1(sp.space_dimension(), EMPTY);
  DProduct known_dp2(sp.space_dimension());
  known_dp2.refine_with_constraint(C == -8);
  known_dp2.refine_with_constraint(c);
  known_dp2.refine_with_constraint(B == 11);

  bool ok = (dp1 == known_dp1 && dp2 == known_dp2);

  ok = ok && sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// is_empty() where both domain objects have points.
bool
test09() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_congruence(A %= 9);
  sp.refine_with_congruence(B + C %= 3);

  bool smash_ok = !sp.is_empty();

  smash_ok = smash_ok && sp.OK();

  CProduct cp(3);
  cp.refine_with_congruence(A %= 9);
  cp.refine_with_congruence(B + C %= 3);

  bool cons_ok = !cp.is_empty();

  cons_ok = cons_ok && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return smash_ok && cons_ok;
}

// is_empty().
bool
test10() {
  Variable A(0);

  SProduct sp(3);
  sp.refine_with_constraint(A <= 1);
  sp.refine_with_constraint(A >= 3);

  bool smash_ok = sp.is_empty();
  smash_ok = smash_ok && sp.OK();

  CProduct cp(3);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 3) / 0);

  bool cons_ok = cp.is_empty();
  cons_ok = cons_ok && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return smash_ok && cons_ok;
}

// is_topologically_closed().
bool
test11() {
  Variable A(0);

  SProduct sp(3);
  bool smash_ok;
  CProduct cp(3);
  bool cons_ok;
#ifdef PH_IS_NNC
  sp.refine_with_constraint(A < 3);
  cp.refine_with_constraint(A < 3);

  smash_ok = !sp.is_topologically_closed();
  cons_ok = !cp.is_topologically_closed();
#else
  sp.refine_with_constraint(A <= 3);
  cp.refine_with_constraint(A <= 3);

  smash_ok = sp.is_topologically_closed();
  cons_ok = cp.is_topologically_closed();
#endif
  smash_ok = smash_ok && sp.OK();
  cons_ok = cons_ok && cp.OK();

  if (!smash_ok || !cons_ok) {
    print_congruences(sp, "*** sp congruences non-empty ***");
    print_constraints(sp, "*** sp constraints non-empty ***");
    print_congruences(cp, "*** cp congruences non-empty ***");
    print_constraints(cp, "*** cp constraints non-empty ***");
    return false;
  }

  sp.refine_with_congruence((A %= 0) / 2);
  sp.refine_with_congruence((A %= 1) / 2);
#ifdef PH_IS_NNC
  cp.refine_with_congruence((A %= 2) / 0);
#else
  cp.refine_with_congruence((A %= 2) / 0);
#endif

  smash_ok = sp.is_topologically_closed();
  cons_ok = cp.is_topologically_closed();
  smash_ok = smash_ok && sp.OK();
  cons_ok = cons_ok && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return smash_ok && cons_ok;
}

// is_disjoint_from().
bool
test12() {
  Variable B(1);

  SProduct sp1(4);
  sp1.refine_with_constraint(B <= 3);
  sp1.refine_with_constraint(B >= 3);

  SProduct sp2(4);
  sp2.refine_with_congruence((B %= 1) / 3);

  bool ok = !sp1.is_disjoint_from(sp2);
  ok = ok && sp1.OK() && sp2.OK();

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  if (!ok)
    return false;

  CProduct cp1(4);
  cp1.refine_with_constraint(B <= 3);
  cp1.refine_with_constraint(B >= 3);

  CProduct cp2(4);
  cp2.refine_with_congruence((B %= 1) / 3);

  ok = cp1.is_disjoint_from(cp2);
  ok = ok && cp1.OK() && cp2.OK();

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return ok;
}

// is_bounded().
bool
test13() {
  Variable A(0);
  Variable B(1);

  bool ok;

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 0);
  sp.refine_with_congruence((A %= 1) / 3);

  ok = sp.is_bounded();
  ok = ok && sp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  if (!ok)
    return false;

  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 1) / 3);
  cp.refine_with_congruence((B %= 1) / 0);

  ok = cp.is_bounded();
  ok = ok && cp.OK();

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// ok(), not reduced.
bool
test14() {
  Variable A(0);
  Variable B(1);

  bool ok;

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 0);
  sp.refine_with_congruence((A %= 1) / 3);

  ok = sp.OK();

#if PH_IS_FIRST
  print_constraints(sp.domain1().constraints(),
		    "*** sp,domain1() constraints ***");
  print_congruences(sp.domain2().congruences(),
		    "*** sp,domain2() congruences ***");
#else
  print_congruences(sp.domain1().congruences(),
		    "*** sp,domain1() congruences ***");
  print_constraints(sp.domain2().constraints(),
		    "*** sp,domain2() constraints ***");
#endif

  if (!ok)
    return false;

  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 1) / 3);
  cp.refine_with_congruence((B %= 1) / 0);

  ok = cp.OK();

#if PH_IS_FIRST
  print_constraints(cp.domain1().constraints(),
		    "*** cp,domain1() constraints ***");
  print_congruences(cp.domain2().congruences(),
		    "*** cp,domain2() congruences ***");
#else
  print_congruences(cp.domain1().congruences(),
		    "*** cp,domain1() congruences ***");
  print_constraints(cp.domain2().constraints(),
		    "*** cp,domain2() constraints ***");
#endif

  return ok;
}

// ok(), is reduced.
bool
test15() {
  Variable A(0);
  Variable B(1);

  bool ok;

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 0);
  sp.refine_with_congruence((A %= 1) / 3);

  // reduce the product
  Constraint_System sp_cs = sp.constraints();

  ok = sp.OK();

  if (!ok) {
    print_constraints(sp_cs, "*** sp.constraints(); ***");
    return false;
  }

  Grid sp_gr(sp_cs);
  ok = sp_gr.is_empty();

#if PH_IS_FIRST
  print_constraints(sp.domain1().constraints(),
		    "*** sp.domain1() constraints ***");
  print_congruences(sp.domain2().congruences(),
		    "*** sp.domain2() congruences ***");
#else
  print_congruences(sp.domain1().congruences(),
		    "*** sp.domain1() congruences ***");
  print_constraints(sp.domain2().constraints(),
		    "*** sp.domain2() constraints ***");
#endif

  if (!ok)
    return false;

  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 1) / 3);
  cp.refine_with_congruence((B %= 1) / 0);

  // reduce the product
  Constraint_System cp_cs = sp.constraints();

  ok = cp.OK();

  if (!ok) {
    print_constraints(cp_cs, "*** cp.constraints(); ***");
    return false;
  }

  Grid cp_gr(cp_cs);
  ok = (cp_gr.affine_dimension() == 0);

#if PH_IS_FIRST
  print_constraints(cp.domain1().constraints(),
		    "*** cp.domain1() constraints ***");
  print_congruences(cp.domain2().congruences(),
		    "*** cp.domain2() congruences ***");
#else
  print_congruences(cp.domain1().congruences(),
		    "*** cp.domain1() congruences ***");
  print_constraints(cp.domain2().constraints(),
		    "*** cp.domain2() constraints ***");
#endif

  return ok;
}

// Building from inequality constraints()
bool
test16() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Constraint_System cs;
  cs.insert(A - C <= 8);
  cs.insert(A - C >= 9);

  try {
    SProduct sp(cs);
   }
  catch (const std::invalid_argument& e) {
    nout << "cs contains an inequality constraint: " << e.what() << endl;
    return true;
  }
  catch (...) {
  }
  return false;
}

// Building from equality congruences()
bool
test17() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Congruence_System cgs1;
  cgs1.insert((A - C %= 8) / 0);
  Congruence_System cgs2;
  cgs2.insert((A - C %= 9) / 0);
  cgs2.insert((B %= 21) / 0);

  SProduct sp(cgs1);
  CProduct cp(cgs2);


  bool ok = sp.OK() && cp.OK();

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// refine_with_congruences
bool
test18() {
  Variable A(0);
  Variable B(1);

  Congruence_System cgs;
  cgs.insert((A %= 0) / 2);
  cgs.insert((A + B %= 0) / 2);
  cgs.insert((B %= 0) / 2);
  cgs.insert(A + B == 0);

  SProduct sp(2);
  CProduct cp(2);

  sp.refine_with_congruences(cgs);
  cp.refine_with_congruences(cgs);

  Grid gr(cgs);

  SProduct known_sp(gr);
  CProduct known_cp(gr);

  bool ok = (sp == known_sp && cp == known_cp);

  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");
  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

  return ok;
}

// add_recycled_congruences
bool
test19() {
  Variable A(0);
  Variable B(1);

  Congruence_System cgs;
  cgs.insert((A + B %= 0) / 2);
  cgs.insert((A %= 0) / 0);

  SProduct sp(2);
  CProduct cp(2);

  Congruence_System cgs_copy = cgs;
  Congruence_System cgs_copy2 = cgs;

  sp.add_recycled_congruences(cgs);
  cp.add_recycled_congruences(cgs_copy);

  Grid gr(cgs_copy2);

  SProduct known_sp(gr);
  CProduct known_cp(gr);

  bool ok = (sp == known_sp && cp == known_cp);

  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");
  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

  return ok;
}

// add_recycled_congruences
bool
test20() {
  Variable A(0);
  Variable B(1);

  Congruence_System cgs;
  cgs.insert((B %= 0) / 2);
  cgs.insert((A %= 0) / 2);
  cgs.insert((A %= 0) / 1);
  cgs.insert(A - B == 0);

  SProduct sp(2);
  CProduct cp(2);

  Congruence_System cgs_copy = cgs;
  Congruence_System cgs_copy2 = cgs;

  sp.add_recycled_congruences(cgs);
  cp.add_recycled_congruences(cgs_copy);

  Grid gr(cgs_copy2);

  SProduct known_sp(gr);
  CProduct known_cp(gr);

  bool ok = (sp == known_sp && cp == known_cp);

  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");
  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

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
  DO_TEST(test20);
END_MAIN
