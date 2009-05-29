/* Test Box::Box(const NNC_Polyhedron&, Complexity_Class).
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

namespace {

// An unbounded NNC polyhedron in 4D but bounded in 2D
// with strict inequality and closure points at the lower bound.
bool
test01() {
  Variable x(1);
  Variable y(2);
  Variable z(3);

  NNC_Polyhedron ph(4);
  ph.add_constraint(3 * x + y > 2);
  ph.add_constraint(x <= 4);
  ph.add_constraint(y <= 4);
  ph.add_constraint(z >= 5);

  TBox pbox(ph, POLYNOMIAL_COMPLEXITY);

  TBox nbox(ph);

  TBox known_nbox(4);
  known_nbox.add_constraint(3*x > -2);
  known_nbox.add_constraint(x <= 4);
  known_nbox.add_constraint(y > -10);
  known_nbox.add_constraint(y <= 4);
  known_nbox.add_constraint(z >= 5);

  TBox known_pbox(4);
  known_pbox.add_constraint(x <= 4);
  known_pbox.add_constraint(y <= 4);
  known_pbox.add_constraint(z >= 5);

  bool ok = (nbox == known_nbox && pbox == known_pbox && pbox.contains(nbox));

  print_constraints(ph, "*** ph ***");
  print_constraints(nbox, "*** nbox ***");
  print_constraints(pbox, "*** pbox ***");

  print_constraints(known_nbox, "*** known_nbox ***");
  print_constraints(known_pbox, "*** known_pbox ***");

  return ok;
}

// A bounded NNC polyhedron with strict inequalities
// causing upper and lower bounds of the box to be open.
bool
test02() {
  Variable x(0);
  Variable y(1);

  NNC_Polyhedron ph(2);
  ph.add_constraint(3 * x + y >= 2);
  ph.add_constraint(x < 4);
  ph.add_constraint(y <= 4);

  TBox pbox(ph, POLYNOMIAL_COMPLEXITY);

  TBox nbox(ph);

  TBox known_nbox(2);
  known_nbox.add_constraint(3*x >= -2);
  known_nbox.add_constraint(x < 4);
  known_nbox.add_constraint(y > -10);
  known_nbox.add_constraint(y <= 4);

  TBox known_pbox(2);
  known_pbox.add_constraint(x < 4);
  known_pbox.add_constraint(y <= 4);

  bool ok = (nbox == known_nbox && pbox == known_pbox && pbox.contains(nbox));

  print_constraints(ph, "*** ph ***");
  print_constraints(nbox, "*** nbox ***");
  print_constraints(pbox, "*** pbox ***");

  print_constraints(known_nbox, "*** known_nbox ***");
  print_constraints(known_pbox, "*** known_pbox ***");

  return ok;
}

// An empty polyhedron in 2D defined using strict constraints.
bool
test03() {
  Variable x(0);
  Variable y(1);
  NNC_Polyhedron ph(2);
  ph.add_constraint(x > 0);
  ph.add_constraint(x < 0);
  ph.add_constraint(y > 0);
  ph.add_constraint(y < 0);

  Rational_Box pbox(ph, POLYNOMIAL_COMPLEXITY);

  Rational_Box nbox(ph);

  NNC_Polyhedron known_ph(2, EMPTY);
  NNC_Polyhedron known_pph(pbox);
  NNC_Polyhedron known_nph(nbox);

  bool ok = (ph == known_ph && ph == known_nph && ph == known_ph);

  print_generators(ph, "*** ph ***");
  print_generators(known_pph, "*** known_pph ***");
  print_generators(known_nph, "*** known_nph ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
END_MAIN
