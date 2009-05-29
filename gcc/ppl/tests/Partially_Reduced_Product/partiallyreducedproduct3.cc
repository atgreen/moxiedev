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

// relation_with a generator
bool
test01() {
  Variable A(0);
  Variable B(1);

  Generator pnt(point(A + B));

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 1);
  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);

  bool smash_ok = Poly_Gen_Relation::subsumes() == sp.relation_with(pnt);
  bool cons_ok = Poly_Gen_Relation::subsumes() == cp.relation_with(pnt);

  if (!smash_ok) {
    print_constraints(sp, "*** sp constraints subsumes ***");
    print_congruences(sp, "*** sp congruences subsumes ***");
    return false;
  }

  if (!cons_ok) {
    print_constraints(cp, "*** cp constraints subsumes ***");
    print_congruences(cp, "*** cp congruences subsumes ***");
    return false;
  }

  sp.refine_with_congruence((A %= 0) / 2);
  cp.refine_with_congruence((A %= 0) / 2);

  smash_ok = Poly_Gen_Relation::nothing() == sp.relation_with(pnt);
  cons_ok = Poly_Gen_Relation::nothing() == cp.relation_with(pnt);

  print_constraints(sp, "*** sp constraints nothing ***");
  print_congruences(sp, "*** sp congruences nothing ***");
  print_constraints(cp, "*** cp constraints nothing ***");
  print_congruences(cp, "*** cp congruences nothing ***");

  return smash_ok && cons_ok;
}

// Empty smash product; relation_with a constraint.
bool
test02() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 1);
  sp.refine_with_congruence((A %= 2) / 0);

  bool ok = (sp.relation_with(B == 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::is_disjoint()
	         && Poly_Con_Relation::saturates())
	     && sp.relation_with(B >= 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::is_disjoint()
	         && Poly_Con_Relation::saturates()));

  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");

  return ok;
}

// Empty constraints product; relation_with a constraint.
bool
test03() {
  Variable A(0);
  Variable B(1);

  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 0) / 2);

  bool ok = (cp.relation_with(B == 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::is_disjoint()
	         && Poly_Con_Relation::saturates())
	     && cp.relation_with(B >= 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::is_disjoint()
	         && Poly_Con_Relation::saturates()));

  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

  return ok;
}

// Non-empty smash product; relation_with a constraint.
bool
test04() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_constraint(3*A + 3*B + C == 7);
  sp.refine_with_constraint(3*A - C >= 2);
  sp.refine_with_congruence(6*A + 3*B %= 0);

  bool ok = (sp.relation_with(A + B + C == 0)
	     == Poly_Con_Relation::nothing()
	     && sp.relation_with(A + B == 0)
	     == Poly_Con_Relation::nothing()
	     && sp.relation_with(A == 0)
	     == Poly_Con_Relation::nothing()
	     && sp.relation_with(Linear_Expression(0) == 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::saturates())
	     && sp.relation_with(2*A + B >= 3)
	     == Poly_Con_Relation::is_included()
	     && sp.relation_with(3*A + 3*B + C >= 7)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::saturates())
	     && sp.relation_with(3*A - C <= 3)
	     == (Poly_Con_Relation::nothing()));

  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");

  return ok;
}

// A non-empty constraints product; relation_with a constraint.
bool
test05() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(3*A + 3*B + C >= 7);
  cp.refine_with_constraint(3*A + 3*B + C <= 7);
  cp.refine_with_constraint(3*A - C >= 2);
  cp.refine_with_congruence(6*A + 3*B %= 0);

  bool ok = (cp.relation_with(A + B + C == 0)
	     == Poly_Con_Relation::nothing()
	     && cp.relation_with(A + B == 0)
	     == Poly_Con_Relation::nothing()
	     && cp.relation_with(A == 0)
	     == Poly_Con_Relation::nothing()
	     && cp.relation_with(Linear_Expression(0) == 0)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::saturates())
	     && cp.relation_with(2*A + B >= 3)
	     == Poly_Con_Relation::is_included()
	     && cp.relation_with(3*A + 3*B + C >= 7)
	     == (Poly_Con_Relation::is_included()
		 && Poly_Con_Relation::saturates())
	     && cp.relation_with(3*A - C <= 3)
	     == (Poly_Con_Relation::nothing()));

  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

  return ok;
}

// Non-empty smash product; relation_with a congruence.
bool
test06() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_constraint(3*A + 3*B + C == 7);
  sp.refine_with_constraint(3*A - C >= 2);
  sp.refine_with_congruence(6*A + 3*B %= 0);

  Poly_Con_Relation rel1 = sp.relation_with(A + B + C %= 0);
  Poly_Con_Relation rel2 = sp.relation_with(A + B %= 0);
  Poly_Con_Relation rel3 = sp.relation_with(A %= 0);
  Poly_Con_Relation rel4 = sp.relation_with(Linear_Expression(0) %= 0);
  Poly_Con_Relation rel5 = sp.relation_with(2*A + B %= 3);
  Poly_Con_Relation rel6 = sp.relation_with(3*A + 3*B + C %= 7);
  Poly_Con_Relation rel7 = sp.relation_with(3*A - C %= 3);
  bool ok = (rel1 == Poly_Con_Relation::nothing()
	     && rel2 == Poly_Con_Relation::nothing()
	     && rel3 == Poly_Con_Relation::nothing()
	     && rel4 == (Poly_Con_Relation::is_included()
			 && Poly_Con_Relation::saturates())
	     && rel5 == Poly_Con_Relation::nothing()
	     && rel6 == (Poly_Con_Relation::is_included()
			 && Poly_Con_Relation::saturates())
	     && rel7 == (Poly_Con_Relation::is_included()));

  nout << "sp.relation_with(A + B + C %= 0) == " << rel1 << endl;
  nout << "sp.relation_with(A + B %= 0) == " << rel2 << endl;
  nout << "sp.relation_with(A %= 0) == " << rel3 << endl;
  nout << "sp.relation_with(Linear_Expression(0) %= 0) == " << rel4 << endl;
  nout << "sp.relation_with(2*A + B %= 3) == " << rel5 << endl;
  nout << "sp.relation_with(3*A + 3*B + C %= 7) == " << rel6 << endl;
  nout << "sp.relation_with(3*A - C %= 3) == " << rel7 << endl;
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(sp, "*** sp congruences ***");

  return ok;
}

// A non-empty constraints product; relation_with a congruence.
bool
test07() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(3*A + 3*B + C >= 7);
  cp.refine_with_constraint(3*A + 3*B + C <= 7);
  cp.refine_with_constraint(3*A - C >= 2);
  cp.refine_with_congruence(6*A + 3*B %= 0);

  Poly_Con_Relation rel1 = cp.relation_with(A + B + C %= 0);
  Poly_Con_Relation rel2 = cp.relation_with(A + B %= 0);
  Poly_Con_Relation rel3 = cp.relation_with(A %= 0);
  Poly_Con_Relation rel4 = cp.relation_with(Linear_Expression(0) %= 0);
  Poly_Con_Relation rel5 = cp.relation_with(2*A + B %= 3);
  Poly_Con_Relation rel6 = cp.relation_with(3*A + 3*B + C %= 7);
  Poly_Con_Relation rel7 = cp.relation_with(3*A - C %= 3);
  bool ok = (rel1 == Poly_Con_Relation::nothing()
	     && rel2 == Poly_Con_Relation::nothing()
	     && rel3 == Poly_Con_Relation::nothing()
	     && rel4 == (Poly_Con_Relation::is_included()
			 && Poly_Con_Relation::saturates())
	     && rel5 == Poly_Con_Relation::nothing()
	     && rel6 == (Poly_Con_Relation::is_included()
			 && Poly_Con_Relation::saturates())
	     && rel7 == (Poly_Con_Relation::is_included()));

  nout << "cp.relation_with(A + B + C %= 0) == " << rel1 << endl;
  nout << "cp.relation_with(A + B %= 0) == " << rel2 << endl;
  nout << "cp.relation_with(A %= 0) == " << rel3 << endl;
  nout << "cp.relation_with(Linear_Expression(0) %= 0) == " << rel4 << endl;
  nout << "cp.relation_with(2*A + B %= 3) == " << rel5 << endl;
  nout << "cp.relation_with(3*A + 3*B + C %= 7) == " << rel6 << endl;
  nout << "cp.relation_with(3*A - C %= 3) == " << rel7 << endl;
  print_constraints(cp, "*** cp constraints ***");
  print_congruences(cp, "*** cp congruences ***");

  return ok;
}

// Smash product; bounds_from_above() and bounds_from_below() when bounded.
bool
test08() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 1);
  sp.refine_with_congruence((A %= 2) / 0);

  Linear_Expression le = A + B;
  bool ok = sp.bounds_from_above(le)
    && sp.bounds_from_below(le);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Constraints product; bounds_from_above() and bounds_from_below()
// when bounded.
bool
  test09() {
  Variable A(0);
  Variable B(1);

  CProduct cp(2);
  cp.refine_with_constraint(A >= 1);
  cp.refine_with_constraint(A <= 1);
  cp.refine_with_congruence((A %= 0) / 2);

  Linear_Expression le = A + B;
  bool ok = cp.bounds_from_above(le)
    && cp.bounds_from_below(le);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// Smash product; bounds_from_above() and bounds_from_below()
// when not bounded.
bool
test10() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 1);
  sp.refine_with_congruence((A %= 0) / 2);

  Linear_Expression le = 2*A - B;

  bool ok = !sp.bounds_from_above(le)
    && !sp.bounds_from_below(le);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Constraints product; bounds_from_above() and bounds_from_below()
// when it is not bounded.
bool
test11() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(A >= 1);
  sp.refine_with_constraint(A <= 1);
  sp.refine_with_congruence((A %= 1) / 2);

  Linear_Expression le = 2*A - B;

  bool ok = !sp.bounds_from_above(le)
    && !sp.bounds_from_below(le);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Smash product; maximize() and minimize() when bounded.
bool
test12() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(A + B >= 1);
  sp.refine_with_constraint(A + B <= 1);
  sp.refine_with_congruence(B %= 0);
  sp.refine_with_congruence((3*A %= 2) / 5);

  Linear_Expression le = A + B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  bool max;
  bool min;

  bool ok = sp.maximize(le, max_n, max_d, max)
    && sp.minimize(le, min_n, min_d, min)
    && sp.maximize(le, max_n, max_d, max, pnt_max)
    && sp.minimize(le, min_n, min_d, min, pnt_min);

  ok = ok
    && max && min && max_n == 1 && max_d == 1 && min_n == 1 && min_d == 1
    && pnt_max == pnt_min;

  print_generator(pnt_max, "*** maximum point ***");

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Constraints product; maximize() and minimize() when bounded.
bool
test13() {
  Variable A(0);
  Variable B(1);

  CProduct cp(2);
  cp.refine_with_constraint(A + B >= 1);
  cp.refine_with_constraint(A + B <= 1);
  cp.refine_with_congruence(B %= 0);

  Linear_Expression le = A + B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  bool max;
  bool min;

  bool ok = cp.maximize(le, max_n, max_d, max)
    && cp.minimize(le, min_n, min_d, min)
    && cp.maximize(le, max_n, max_d, max, pnt_max)
    && cp.minimize(le, min_n, min_d, min, pnt_min);

  ok = ok
    && max && min && max_n == 1 && max_d == 1 && min_n == 1 && min_d == 1
    && pnt_max == pnt_min;

  print_generator(pnt_max, "*** maximum point ***");

  if (!ok) {
    print_congruences(cp, "*** cp congruences non-empty product ***");
    print_constraints(cp, "*** cp constraints non-empty product ***");
    return false;
  }

  cp.refine_with_congruence((3*A %= 2) / 3);

  ok = !cp.maximize(le, max_n, max_d, max)
    && !cp.minimize(le, min_n, min_d, min)
    && !cp.maximize(le, max_n, max_d, max, pnt_max)
    && !cp.minimize(le, min_n, min_d, min, pnt_min);

  print_congruences(cp, "*** cp congruences empty product ***");
  print_constraints(cp, "*** cp constraints empty product ***");

  return ok;
}

// Smash and constraints product; maximize() and minimize()
// when not bounded.
bool
test14() {
  Variable A(0);
  Variable B(1);

  SProduct sp(2);
  sp.refine_with_constraint(B >= 1);
  sp.refine_with_constraint(B <= 2);
  sp.refine_with_congruence((B %= 3) / 5);

  CProduct cp(2);
  cp.refine_with_constraint(B >= 1);
  cp.refine_with_constraint(B <= 2);
  cp.refine_with_congruence((B %= 3) / 5);

  Linear_Expression le = 2*A - B;

  Coefficient max_n;
  Coefficient max_d;
  Coefficient min_n;
  Coefficient min_d;
  Generator pnt_max(point());
  Generator pnt_min(point());
  bool max;
  bool min;

  bool smash_ok = !sp.maximize(le, max_n, max_d, max)
    && !sp.minimize(le, min_n, min_d, min)
    && !sp.maximize(le, max_n, max_d, max, pnt_max)
    && !sp.minimize(le, min_n, min_d, min, pnt_min);

  bool cons_ok = !cp.maximize(le, max_n, max_d, max)
    && !cp.minimize(le, min_n, min_d, min)
    && !cp.maximize(le, max_n, max_d, max, pnt_max)
    && !cp.minimize(le, min_n, min_d, min, pnt_min);

  print_constraints(sp, "*** sp constraints ***");
  print_constraints(sp, "*** sp constraints ***");
  print_congruences(cp, "*** cp congruences ***");
  print_congruences(cp, "*** cp congruences ***");

  return smash_ok && cons_ok;
}

// Smash product; bounded_affine_image/3
bool
test15() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_congruence((A ==  0) / 0);
  sp.refine_with_congruence((B ==  0) / 0);
  sp.refine_with_congruence((C == -2) / 0);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  sp.bounded_affine_image(A, 7-B, B+3);

  SProduct known_sp(3);
  known_sp.refine_with_constraint(C == -2);
  known_sp.refine_with_constraint(B == 0);
  known_sp.refine_with_constraint(A <= 3);
  known_sp.refine_with_constraint(A + B >= 7);

  bool ok = (sp == known_sp);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Constraints product; bounded_affine_image/3.
bool
test16() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(A >=  0);
  cp.refine_with_constraint(A <=  0);
  cp.refine_with_constraint(B >=  0);
  cp.refine_with_constraint(B <=  0);
  cp.refine_with_congruence((C == -2) / 0);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  cp.bounded_affine_image(A, 7-B, B+3);

  CProduct known_cp(3);
  known_cp.refine_with_constraint(C == -2);
  known_cp.refine_with_constraint(B == 0);
  known_cp.refine_with_constraint(A <= 3);
  known_cp.refine_with_constraint(A + B >= 7);

  bool ok = (cp == known_cp);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// Smash product; bounded_affine_preimage/3.
bool
test17() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_congruence((A ==  0) / 0);
  sp.refine_with_congruence((B ==  0) / 0);
  sp.refine_with_congruence((C == -2) / 0);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  sp.bounded_affine_preimage(A, 7-B, B+3);

  Constraint_System cs;
  cs.insert(C == -2);
  cs.insert(B == 0);
  cs.insert(C >= 3);

  SProduct known_sp(3);
  known_sp.refine_with_constraints(cs);

  bool ok = (sp == known_sp);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return ok;
}

// Constraints product; bounded_affine_preimage/3.
bool
test18() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(A >=  0);
  cp.refine_with_constraint(A <=  0);
  cp.refine_with_constraint(B >=  0);
  cp.refine_with_constraint(B <=  0);
  cp.refine_with_congruence((C == -2) / 0);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  cp.bounded_affine_preimage(A, 7-B, B+3);

  Constraint_System cs;
  cs.insert(C == -2);
  cs.insert(B == 0);
  cs.insert(C >= 3);

  CProduct known_cp(3);
  known_cp.refine_with_constraints(cs);

  bool ok = (cp == known_cp);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return ok;
}

// concatenate_assign()
bool
test19() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  SProduct sp1(2);
  sp1.refine_with_constraint(A >= 0);
  sp1.refine_with_congruence((A %= 0) / 2);

  // Make sure sp1 is reduced;
  (void) sp1.is_topologically_closed();

  SProduct sp2(2);
  // Make sure sp2 is not reduced;
  sp2.refine_with_constraint(A <= 1);
  sp2.refine_with_constraint(A >= 2);
  sp2.refine_with_constraint(B >= 0);

  sp1.concatenate_assign(sp2);

  SProduct known_sp(4, EMPTY);

  bool ok = (sp1 == known_sp) && sp1.OK();

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  return ok;
}

bool
test20() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  CProduct cp1(2);
  cp1.refine_with_constraint(A >= 0);
  cp1.refine_with_congruence((A %= 0) / 2);

  // Make sure cp1 is reduced;
  (void) cp1.is_topologically_closed();

  CProduct cp2(2);
  // Make sure cp2 is not reduced;
  cp2.refine_with_constraint(A <= 1);
  cp2.refine_with_constraint(A >= 1);
  cp2.refine_with_constraint(B >= 0);

  cp1.concatenate_assign(cp2);

  CProduct known_cp(4);
  known_cp.refine_with_constraint(A >= 0);
  known_cp.refine_with_congruence((A %= 0) / 2);
  known_cp.refine_with_constraint(C == 1);
  known_cp.refine_with_constraint(D >= 0);

  bool ok = (cp1 == known_cp) && cp1.OK();

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

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
