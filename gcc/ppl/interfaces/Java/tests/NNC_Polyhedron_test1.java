/* Test NNC_Polyhedron Java test class of the Parma Polyhedra Library Java
   interface.
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

import java.math.BigInteger;
import java.util.Iterator;
import java.util.Vector;
import parma_polyhedra_library.*;


public class NNC_Polyhedron_test1 {
static {
    try {
        System.loadLibrary("ppl_java");
    }

   catch (UnsatisfiedLinkError  e) {
       System.out.println("Unable to load the library");
       System.exit(-1);
   }
}

    public static Boolean test01() {
	Variable X = new Variable(0);
	Variable Y = new Variable(1);
	Variable Z = new Variable(2);
	NNC_Polyhedron ph = new NNC_Polyhedron(3, Degenerate_Element.UNIVERSE);
	Linear_Expression le_X = new Linear_Expression_Variable(X);
	Linear_Expression le_Y = new Linear_Expression_Variable(Y);
	Linear_Expression le_Z = new Linear_Expression_Variable(Z);
	Linear_Expression le_2Y = le_Y.times(new Coefficient(2));
	Linear_Expression le_5Z = le_Z.times(new Coefficient(5));
	Linear_Expression le_7
	    = new Linear_Expression_Coefficient(new Coefficient(7));
	Linear_Expression le_5
	    = new Linear_Expression_Coefficient(new Coefficient(5));
	Linear_Expression lhs1 = le_X.sum(le_2Y.sum(le_5Z));
	NNC_Polyhedron ph1 = new NNC_Polyhedron(3,
						Degenerate_Element.UNIVERSE);
	ph1.add_constraint(new Constraint(lhs1,
					 Relation_Symbol.GREATER_OR_EQUAL,
					 le_7));
	ph1.add_constraint(new Constraint(le_X, Relation_Symbol.LESS_THAN,
					  le_5Z));
	PPL_Test.println_if_noisy(ph1.constraints().toString());
	return new Boolean(true);
    }

    public static Boolean test02() {
	// Test if `minimized_constraints' returns an empty Constraint_System
	// if the Polyhedron is built from universe with a dimension greater
	// than zero.
	Variable X = new Variable(0);
	Variable Y = new Variable(1);
	Variable Z = new Variable(2);
	NNC_Polyhedron ph = new NNC_Polyhedron(3, Degenerate_Element.UNIVERSE);
	Constraint_System cs = ph.minimized_constraints();
	return new Boolean(cs.isEmpty());
    }

    public static void main(String[] args) {
	boolean test_result_ok =
	    Test_Executor.executeTests(NNC_Polyhedron_test1.class);
	if (!test_result_ok)
	    System.exit(1);
	System.exit(0);
    }

}
