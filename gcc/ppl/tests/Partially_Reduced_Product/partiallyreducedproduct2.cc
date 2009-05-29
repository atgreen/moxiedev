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

#define PH_IS_NNC
// #define PH_IS_FIRST

#ifdef PH_IS_NNC
typedef NNC_Polyhedron Poly;
#else
typedef C_Polyhedron Poly;
#endif

#ifdef PH_IS_FIRST
typedef Domain_Product<Poly, Grid>::Smash_Product SProduct;
typedef Domain_Product<Poly, Grid>::Constraints_Product CProduct;
#else
typedef Domain_Product<Grid, Poly>::Smash_Product SProduct;
typedef Domain_Product<Grid, Poly>::Constraints_Product CProduct;
#endif

namespace {

// Tests 1-10 are for just the Smash_Product domains.

// space_dimension()
bool
test01() {
  Variable A(0);
  Variable E(4);

#ifdef PH_IS_NNC
  Constraint_System cs(A + E < 9);
#else
  Constraint_System cs(A + E <= 9);
#endif

  SProduct sp(5);
  sp.refine_with_constraints(cs);

  bool smash_ok = (sp.space_dimension() == 5);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return smash_ok;
}

// affine_dimension()
bool
test02() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_constraint(A - C <= 9);
  sp.refine_with_constraint(A - C >= 9);
  sp.refine_with_constraint(B >= 2);

  bool smash_ok = sp.space_dimension() == 3;
  smash_ok = smash_ok && sp.affine_dimension() == 2;

  if (smash_ok) {
    sp.refine_with_constraint(A - C >= 4);
    sp.refine_with_constraint(A - C <= 4);

    smash_ok = smash_ok && sp.affine_dimension() == 0;
  }

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return smash_ok;
}

// contains()
bool
test03() {
  Variable A(0);

  SProduct sp1(1);
  sp1.refine_with_constraint(A <= 3);
  sp1.refine_with_congruence((A %= 1) / 2);

  SProduct sp2(1);
  sp2.refine_with_constraint(A <= 3);
  sp2.refine_with_constraint(A >= 4);
  sp2.refine_with_congruence((A %= 0) / 2);

  bool smash_ok1 = sp1.contains(sp2) && sp1.strictly_contains(sp2);

  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp1, "*** sp1 congruences ***");

  if (!smash_ok1)
    return smash_ok1;

  SProduct sp3(1);
  sp3.refine_with_constraint(A <= 3);
  sp3.refine_with_congruence((A %= 1) / 2);

  SProduct sp4(1);
  sp4.refine_with_constraint(A <= 4);
  sp4.refine_with_congruence((A %= 0) / 2);
  sp4.refine_with_congruence((A %= 1) / 2);

  bool smash_ok2 = sp3.contains(sp4) && sp3.strictly_contains(sp4);

  print_constraints(sp3, "*** sp3 constraints ***");
  print_congruences(sp3, "*** sp3 congruences ***");

  if (!smash_ok2)
    return smash_ok2;

  SProduct sp5(1);
  sp5.refine_with_constraint(A <= 3);
  sp5.refine_with_constraint(A >= 4);
  sp5.refine_with_congruence((A %= 1) / 2);
  sp5.refine_with_congruence((A %= 0) / 2);

  SProduct sp6(1, EMPTY);

  bool smash_ok3 = sp5.contains(sp6) && !sp5.strictly_contains(sp6);

  print_constraints(sp5, "*** sp5 constraints ***");
  print_congruences(sp5, "*** sp5 congruences ***");

  return smash_ok3;
}

// intersection_assign()
bool
test04() {
  Variable A(0);
  Variable B(1);

  SProduct sp1(3);
  sp1.refine_with_constraint(A >= 0);
  sp1.refine_with_congruence((A %= 0) / 2);

  SProduct sp2(3);
  sp2.refine_with_constraint(A <= 0);
  sp2.refine_with_congruence((A %= 0) / 1);

  sp1.intersection_assign(sp2);

  SProduct known_sp(3);
  known_sp.refine_with_constraint(A == 0);

  bool smash_ok1 = !(sp1 == known_sp);
  if (!smash_ok1) {
    print_congruences(sp1, "*** sp1 congruences ***");
    print_constraints(sp1, "*** sp1 constraints ***");
    print_congruences(sp2, "*** sp2 congruences ***");
    print_constraints(sp2, "*** sp2 constraints ***");
    return smash_ok1;
  }

  sp2.refine_with_constraint(A >= 1);
  sp1.intersection_assign(sp2);

  known_sp.refine_with_constraint(A == 1);

  bool smash_ok2 = (sp1 == known_sp);

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  return smash_ok2;
}

// upper_bound_assign(sp2)
bool
test05() {
  Variable A(0);
  Variable B(1);

  SProduct sp(1);
  sp.refine_with_constraint(A <= 18);
  sp.refine_with_constraint(A >= 18);

  SProduct sp1(sp);

  SProduct sp2(1);
  sp2.refine_with_congruence((A %= 19) / 20);
  sp2.refine_with_congruence((A %= 18) / 20);

  sp1.upper_bound_assign(sp2);

  bool smash_ok = (sp1 == sp);

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");

  if (!smash_ok)
    return false;

  sp2.upper_bound_assign(sp1);

  smash_ok = (sp2 == sp);

  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  return smash_ok;
}

// upper_bound_assign_if_exact()
bool
test06() {
  Variable A(0);
  Variable B(1);

  SProduct sp(3);
  sp.refine_with_congruence((A %= 0) / 2);
  sp.refine_with_congruence((B %= 0) / 2);

  SProduct sp1(sp);

  SProduct sp2(3);
  sp2.refine_with_congruence((A %= 1) / 3);
  sp2.refine_with_congruence((B %= 1) / 3);

  bool smash_ok = (!sp1.upper_bound_assign_if_exact(sp2)
		   && !sp2.upper_bound_assign_if_exact(sp1));

  if (!smash_ok) {
    print_congruences(sp1, "*** sp1 congruences ub exact ***");
    print_constraints(sp1, "*** sp1 constraints ub exact ***");
    print_congruences(sp2, "*** sp2 congruences ub exact ***");
    print_constraints(sp2, "*** sp2 constraints ub exact ***");
    return false;
  }

  smash_ok = (sp == sp1);

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  if (!smash_ok)
    return false;

  // The grid component is empty so that the smash product
  // reduces the polyhedron component to empty.
  sp1.refine_with_congruence((B %= 1) / 2);
  sp1.refine_with_congruence((B %= 0) / 2);

  // The polyhedron component is empty so that the smash product
  // reduces the grid component to empty.
  sp2.refine_with_constraint(B >= 1);
  sp2.refine_with_constraint(B <= 0);

  smash_ok = sp1.upper_bound_assign_if_exact(sp2)
   && sp2.upper_bound_assign_if_exact(sp1);

  if (!smash_ok)
    return false;

  SProduct known_sp(3, EMPTY);

  smash_ok = (sp1 == known_sp) && (sp2 == known_sp);

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");
  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  return (smash_ok);
}

// difference_assign(sp2)
bool
test07() {
  Variable A(0);
  Variable B(1);

  SProduct sp(1);
  sp.refine_with_constraint(A <= 18);
  sp.refine_with_constraint(A >= 18);

  SProduct sp1(sp);

  SProduct sp2(1);
  sp2.refine_with_congruence((A %= 19) / 20);
  sp2.refine_with_congruence((A %= 18) / 20);

  sp1.difference_assign(sp2);

  bool smash_ok = (sp1 == sp);

  print_congruences(sp1, "*** sp1 congruences ***");
  print_constraints(sp1, "*** sp1 constraints ***");

  if (!smash_ok)
    return false;

  sp2.difference_assign(sp1);

  SProduct known_sp(1, EMPTY);

  smash_ok = (sp2 == known_sp);

  print_congruences(sp2, "*** sp2 congruences ***");
  print_constraints(sp2, "*** sp2 constraints ***");

  return smash_ok;
}

// time_elapse_assign(y)
bool
test08() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp1(3);
  sp1.refine_with_constraint(A >= 0);
  sp1.refine_with_constraint(B >= 0);
  sp1.refine_with_constraint(A + B >= 3);
  sp1.refine_with_constraint(2*A - B == 0);
  sp1.refine_with_constraint(3*A + C == 0);
  sp1.refine_with_congruence(3*A %= 0);

  SProduct sp2(3);
  sp2.refine_with_constraint(7*C == 4);
  sp2.refine_with_constraint(7*B == -1);
  sp2.refine_with_constraint(7*A == 3);

  sp1.time_elapse_assign(sp2);

  SProduct known_sp(3);
  known_sp.refine_with_constraint(5*A - 13*B - 7*C == 0);
  known_sp.refine_with_constraint(3*A + C >= 0);
  known_sp.refine_with_constraint(A + B >= 3);
  known_sp.refine_with_constraint(4*A - 3*C >= 13);
  known_sp.refine_with_congruence((65*A - B %= 0) / 7);
  known_sp.refine_with_congruence(21*A %= 0);

  bool smash_ok = (sp1 == known_sp);

  print_congruences(sp1, "*** sp1.time_elapse_assign(sp2) congruences ***");
  print_constraints(sp1, "*** sp1.time_elapse_assign(sp2) constraints ***");
  print_congruences(sp2, "*** sp2.time_elapse_assign(sp2) congruences ***");
  print_constraints(sp2, "*** sp2.time_elapse_assign(sp2) constraints ***");

  return smash_ok;
}

// topological_closure_assign
bool
test09() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp(3);
  sp.refine_with_constraint(B >= 0);
  sp.refine_with_constraint(3*A + C == 0);
  sp.refine_with_constraint(2*A - B == 0);
  sp.refine_with_congruence(3*A %= 0);
#ifdef PH_IS_NNC
  sp.refine_with_constraint(A > 0);
#else
  sp.refine_with_constraint(A >= 0);
#endif

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  sp.topological_closure_assign();

  SProduct known_sp(3);
  known_sp.refine_with_constraint(B >= 0);
  known_sp.refine_with_constraint(3*A + C == 0);
  known_sp.refine_with_constraint(2*A - B == 0);
  known_sp.refine_with_congruence(3*A %= 0);
  known_sp.refine_with_constraint(A >= 0);

  bool smash_ok = (sp == known_sp);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  return smash_ok;
}

// widening_assign
bool
test10() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  SProduct sp_prev(3);
  sp_prev.refine_with_constraint(C == 0);
  sp_prev.refine_with_constraint(A - B >= 1);
  sp_prev.refine_with_constraint(A <= 2);
  sp_prev.refine_with_constraint(B >= 0);
  sp_prev.refine_with_congruence((B %= 0) / 2);
  sp_prev.refine_with_congruence(3*A %= 0);

  print_congruences(sp_prev, "*** sp_prev congruences ***");
  print_constraints(sp_prev, "*** sp_prev constraints ***");

  SProduct sp(3);
  sp.refine_with_constraint(C == 0);
  sp.refine_with_constraint(A <= 2);
  sp.refine_with_constraint(B >= 0);
  sp.refine_with_constraint(2*A - B >= 2);
  sp.refine_with_constraint(B >= 0);
  sp.refine_with_congruence(6*A %= 0);
  sp.refine_with_congruence((B %= 0) / 2);

  sp.upper_bound_assign(sp_prev);

  print_congruences(sp, "*** sp congruences ***");
  print_constraints(sp, "*** sp constraints ***");

  sp.widening_assign(sp_prev);

  SProduct known_sp(3);
  known_sp.refine_with_constraint(C == 0);
  known_sp.refine_with_constraint(A <= 2);
  known_sp.refine_with_constraint(B >= 0);
  known_sp.refine_with_congruence((B %= 0) / 2);

  bool smash_ok = (sp == known_sp);

  print_congruences(sp, "*** sp.widening_assign(sp_prev) congruences ***");
  print_constraints(sp, "*** sp.widening_assign(sp_prev) constraints ***");

  return smash_ok;
}

// Tests 11-20 are for just the Constraints_Product domain.

// space_dimension()
bool
test11() {
  Variable A(0);
  Variable E(4);

  Constraint_System cs;
  cs.insert(A + E <= 9);
  cs.insert(A + E >= 9);

  CProduct cp(5);
  cp.refine_with_constraints(cs);

  bool cons_ok = (cp.space_dimension() == 5);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return cons_ok;
}

// affine_dimension()
bool
test12() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(A - C <= 9);
  cp.refine_with_constraint(A - C >= 9);
  cp.refine_with_constraint(B >= 2);

  bool cons_ok = cp.space_dimension() == 3;
  cons_ok = cons_ok && cp.affine_dimension() == 2;

  if (cons_ok) {
    cp.refine_with_constraint(A - B >= 4);
    cp.refine_with_constraint(A - B <= 4);

    cons_ok = cons_ok && cp.affine_dimension() == 1;
  }

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return cons_ok;
}

// contains()
bool
test13() {
  Variable A(0);

  CProduct cp1(1);
  cp1.refine_with_constraint(A <= 3);
  cp1.refine_with_congruence((A %= 1) / 2);

  CProduct cp2(1);
  cp2.refine_with_constraint(A <= 2);
  cp2.refine_with_constraint(A >= 2);
  cp2.refine_with_congruence((A %= 1) / 4);

  bool cons_ok1 = cp1.contains(cp2) && cp1.strictly_contains(cp2);

  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp1, "*** cp1 congruences ***");

  if (!cons_ok1)
    return cons_ok1;

  CProduct cp3(1);
  cp3.refine_with_constraint(A <= 3);
  cp3.refine_with_congruence((A %= 1) / 0);

  CProduct cp4(1);
  cp4.refine_with_constraint(A == 1);

  bool cons_ok2 = cp3.contains(cp4) && !cp3.strictly_contains(cp4);

  print_constraints(cp3, "*** cp3 constraints ***");
  print_congruences(cp3, "*** cp3 congruences ***");

  if (!cons_ok2)
    return cons_ok2;

  CProduct cp5(1);
  cp5.refine_with_constraint(A <= 3);
  cp5.refine_with_constraint(A >= 4);
  cp5.refine_with_congruence((A %= 1) / 2);
  cp5.refine_with_congruence((A %= 0) / 2);

  CProduct cp6(1, EMPTY);

  bool cons_ok3 = cp5.contains(cp6) && !cp5.strictly_contains(cp6);

  print_constraints(cp5, "*** cp5 constraints ***");
  print_congruences(cp5, "*** cp5 congruences ***");

  return cons_ok3;
}

// intersection_assign()
bool
test14() {
  Variable A(0);
  Variable B(1);

  CProduct cp1(3);
  cp1.refine_with_constraint(A >= 0);
  cp1.refine_with_congruence((A %= 0) / 2);

  CProduct cp2(3);
  cp2.refine_with_constraint(A <= 0);
  cp2.refine_with_congruence((A %= 0) / 2);

  cp1.intersection_assign(cp2);

  CProduct known_cp(3);
  known_cp.refine_with_constraint(A == 0);
  known_cp.refine_with_congruence((A %= 0) / 2);

  bool cons_ok1 = (cp1 == known_cp);
  if (!cons_ok1) {
    print_congruences(cp1, "*** cp1 congruences ***");
    print_constraints(cp1, "*** cp1 constraints ***");
    print_congruences(cp2, "*** cp2 congruences ***");
    print_constraints(cp2, "*** cp2 constraints ***");
    return cons_ok1;
  }

  cp2.refine_with_constraint(A >= 1);
  cp1.intersection_assign(cp2);

  known_cp.refine_with_constraint(A == 1);

  bool cons_ok2 = (cp1 == known_cp);

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return cons_ok2;
}

// upper_bound_assign(cp2)
bool
test15() {
  Variable A(0);

  CProduct cp(1);
  cp.refine_with_constraint(A <= 18);
  cp.refine_with_constraint(A >= 18);

  CProduct cp1(cp);

  CProduct cp2(1);
  cp2.refine_with_congruence((A %= 18) / 20);

  cp1.upper_bound_assign(cp2);

  CProduct known_cp(1);
  known_cp.refine_with_congruence((A %= 18) / 20);

  bool cons_ok = (cp1 == known_cp);

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  if (!cons_ok) {
    return false;
  }

  cp2.upper_bound_assign(cp1);

  cons_ok = (cp2 == known_cp);

  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return cons_ok;
}

// upper_bound_assign_if_exact()
bool
test16() {
  Variable A(0);
  Variable B(1);

  CProduct cp(3);
  cp.refine_with_congruence((A %= 0) / 2);
  cp.refine_with_congruence((B %= 0) / 2);

  CProduct cp1(cp);

  CProduct cp2(3);
  cp2.refine_with_congruence((A %= 1) / 3);
  cp2.refine_with_congruence((B %= 1) / 3);

  bool cons_ok = (!cp1.upper_bound_assign_if_exact(cp2)
		   && !cp2.upper_bound_assign_if_exact(cp1));

  if (!cons_ok) {
    print_congruences(cp1, "*** cp1 congruences ub exact ***");
    print_constraints(cp1, "*** cp1 constraints ub exact ***");
    print_congruences(cp2, "*** cp2 congruences ub exact ***");
    print_constraints(cp2, "*** cp2 constraints ub exact ***");
    return false;
  }

  cons_ok = (cp == cp1);

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  if (!cons_ok)
    return false;

  cp1.refine_with_congruence((B %= 1) / 2);

  cp2.refine_with_constraint(B >= 1);
  cp2.refine_with_constraint(B <= 1);

  cons_ok = cp1.upper_bound_assign_if_exact(cp2)
   && cp2.upper_bound_assign_if_exact(cp1);

  if (!cons_ok)
    return false;

  CProduct known_cp(3);
  known_cp.refine_with_congruence((B %= 1) / 0);
  known_cp.refine_with_congruence((A %= 1) / 3);

  cons_ok = (cp1 == known_cp) && (cp2 == known_cp);

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");
  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return (cons_ok);
}

// difference_assign(cp2)
bool
test17() {
  Variable A(0);
  Variable B(1);

  CProduct cp(1);
  cp.refine_with_constraint(A <= 18);
  cp.refine_with_constraint(A >= 18);

  CProduct cp1(cp);

  CProduct cp2(1);
  cp2.refine_with_congruence((A %= 18) / 20);

  cp1.difference_assign(cp2);

  bool cons_ok = cp1.is_empty();

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  if (!cons_ok)
    return false;

  cp2.difference_assign(cp1);

  CProduct known_cp(1);
  known_cp.refine_with_congruence((A %= 18) / 20);

  cons_ok = (cp2 == known_cp);

  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return cons_ok;
}

// time_elapse_assign(y)
bool
test18() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp1(3);
  cp1.refine_with_constraint(A >= 0);
  cp1.refine_with_constraint(B >= 0);
  cp1.refine_with_constraint(A + B >= 3);
  cp1.refine_with_constraint(2*A - B == 0);
  cp1.refine_with_constraint(3*A + C == 0);
  cp1.refine_with_congruence(3*A %= 0);

  CProduct cp2(3);
  cp2.refine_with_constraint(7*C == 4);
  cp2.refine_with_constraint(7*B == -1);
  cp2.refine_with_constraint(7*A == 3);

  cp1.time_elapse_assign(cp2);

  CProduct known_cp(3);
  known_cp.refine_with_constraint(5*A - 13*B - 7*C == 0);
  known_cp.refine_with_constraint(3*A + C >= 0);
  known_cp.refine_with_constraint(A + B >= 3);
  known_cp.refine_with_constraint(4*A - 3*C >= 13);
  known_cp.refine_with_congruence((65*A - B %= 0) / 7);
  known_cp.refine_with_congruence(21*A %= 0);

  bool cons_ok = (cp1 == known_cp);

  print_congruences(cp1, "*** cp1.time_elapse_assign(cp2) congruences ***");
  print_constraints(cp1, "*** cp1.time_elapse_assign(cp2) constraints ***");
  print_congruences(cp2, "*** cp2.time_elapse_assign(cp2) congruences ***");
  print_constraints(cp2, "*** cp2.time_elapse_assign(cp2) constraints ***");

  return cons_ok;
}

// topological_closure_assign
bool
test19() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp(3);
  cp.refine_with_constraint(B >= 0);
  cp.refine_with_constraint(3*A + C == 0);
  cp.refine_with_constraint(2*A - B == 0);
  cp.refine_with_congruence(3*A %= 0);
#ifdef PH_IS_NNC
  cp.refine_with_constraint(A > 0);
#else
  cp.refine_with_constraint(A >= 0);
#endif

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  cp.topological_closure_assign();

  CProduct known_cp(3);
  known_cp.refine_with_constraint(B >= 0);
  known_cp.refine_with_constraint(3*A + C == 0);
  known_cp.refine_with_constraint(2*A - B == 0);
  known_cp.refine_with_congruence(3*A %= 0);
  known_cp.refine_with_constraint(A >= 0);

  bool cons_ok = (cp == known_cp);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  return cons_ok;
}

// widening_assign
bool
test20() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  CProduct cp_prev(3);
  cp_prev.refine_with_constraint(C == 0);
  cp_prev.refine_with_constraint(A - B >= 1);
  cp_prev.refine_with_constraint(A <= 2);
  cp_prev.refine_with_constraint(B >= 0);
  cp_prev.refine_with_congruence((B %= 0) / 2);
  cp_prev.refine_with_congruence(3*A %= 0);

  print_congruences(cp_prev, "*** cp_prev congruences ***");
  print_constraints(cp_prev, "*** cp_prev constraints ***");

  CProduct cp(3);
  cp.refine_with_constraint(C == 0);
  cp.refine_with_constraint(A <= 2);
  cp.refine_with_constraint(B >= 0);
  cp.refine_with_constraint(2*A - B >= 2);
  cp.refine_with_constraint(B >= 0);
  cp.refine_with_congruence(6*A %= 0);
  cp.refine_with_congruence((B %= 0) / 2);

  cp.upper_bound_assign(cp_prev);

  print_congruences(cp, "*** cp congruences ***");
  print_constraints(cp, "*** cp constraints ***");

  cp.widening_assign(cp_prev);

  CProduct known_cp(3);
  known_cp.refine_with_constraint(C == 0);
  known_cp.refine_with_constraint(A <= 2);
  known_cp.refine_with_constraint(B >= 0);
  known_cp.refine_with_congruence((B %= 0) / 2);

  bool cons_ok = (cp == known_cp);

  print_congruences(cp, "*** cp.widening_assign(cp_prev) congruences ***");
  print_constraints(cp, "*** cp.widening_assign(cp_prev) constraints ***");

  return cons_ok;
}

// time_elapse_assign(y) where the initial products are not reduced
// and only one component of the second product is empty.
bool
test21() {
  Variable A(0);

  CProduct cp1(1);
  cp1.refine_with_constraint(A == A);

  CProduct cp2(1);
  cp2.refine_with_constraint(A >= 1);
  cp2.refine_with_constraint(A <= 0);

  cp1.time_elapse_assign(cp2);

  CProduct known_cp(1, EMPTY);

  bool cons_ok = cp1.OK() && (cp1 == known_cp);

  print_congruences(cp1, "*** cp1.time_elapse_assign(cp2) congruences ***");
  print_constraints(cp1, "*** cp1.time_elapse_assign(cp2) constraints ***");

  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return cons_ok;
}

// time_elapse_assign(y) where the initial products are not reduced
// and the second product has non-intersecting single point components.
bool
test22() {
  Variable A(0);


  CProduct cp1(1);

  print_congruences(cp1, "*** cp1 congruences ***");
  print_constraints(cp1, "*** cp1 constraints ***");

  Grid gr(1);
  gr.refine_with_congruence((A %= 0)/ 2);

  CProduct cp2(gr);

  cp2.refine_with_constraint(A >= 1);
  cp2.refine_with_constraint(A <= 1);

  cp1.time_elapse_assign(cp2);

  CProduct known_cp(1, EMPTY);

  bool cons_ok = cp1.OK() && (cp1 == known_cp);

  print_congruences(cp1, "*** cp1.time_elapse_assign(cp2) congruences ***");
  print_constraints(cp1, "*** cp1.time_elapse_assign(cp2) constraints ***");

  print_congruences(cp2, "*** cp2 congruences ***");
  print_constraints(cp2, "*** cp2 constraints ***");

  return cons_ok;
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
  DO_TEST_F8(test08);
  DO_TEST(test09);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST(test17);
  DO_TEST_F8(test18);
  DO_TEST(test19);
  DO_TEST(test20);
  DO_TEST(test21);
  DO_TEST(test22);
END_MAIN
