# Automake source file for the Parma Polyhedra Library.
# Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>
#
# This file is part of the Parma Polyhedra Library (PPL).
#
# The PPL is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# The PPL is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
#
# For the most up-to-date information see the Parma Polyhedra Library
# site: http://www.cs.unipr.it/ppl/ .

JAVAROOT = ../

required_instantiations_java_sources = \
@required_instantiations_java_sources@

required_instantiations_java_cxx_headers = \
@required_instantiations_java_cxx_headers@

interface_generator_files = \
ppl_interface_generator_java_classes_java.m4 \
ppl_interface_generator_java_classes_java_code.m4

fixed_java_cxx_headers = \
parma_polyhedra_library.Congruence \
parma_polyhedra_library.Congruence_System \
parma_polyhedra_library.Constraint \
parma_polyhedra_library.Constraint_System \
parma_polyhedra_library.Generator \
parma_polyhedra_library.Generator_System \
parma_polyhedra_library.Grid_Generator \
parma_polyhedra_library.Grid_Generator_System \
parma_polyhedra_library.IO \
parma_polyhedra_library.Linear_Expression \
parma_polyhedra_library.MIP_Problem \
parma_polyhedra_library.Parma_Polyhedra_Library

fixed_java_cxx_headers_sources = \
parma_polyhedra_library_Congruence.h \
parma_polyhedra_library_Congruence_System.h \
parma_polyhedra_library_Constraint.h \
parma_polyhedra_library_Constraint_System.h \
parma_polyhedra_library_Generator.h \
parma_polyhedra_library_Generator_System.h \
parma_polyhedra_library_Grid_Generator.h \
parma_polyhedra_library_Grid_Generator_System.h \
parma_polyhedra_library_IO.h \
parma_polyhedra_library_Linear_Expression.h \
parma_polyhedra_library_MIP_Problem.h \
parma_polyhedra_library_Parma_Polyhedra_Library.h

# NOTE: do _NOT_ add Fake_Class_for_Doxygen.java to this list.
fixed_java_sources = \
$(srcdir)/By_Reference.java \
$(srcdir)/Coefficient.java \
$(srcdir)/Complexity_Class.java \
$(srcdir)/Congruence.java \
$(srcdir)/Congruence_System.java \
$(srcdir)/Constraint.java \
$(srcdir)/Constraint_System.java \
$(srcdir)/Control_Parameter_Name.java \
$(srcdir)/Control_Parameter_Value.java \
$(srcdir)/Degenerate_Element.java \
$(srcdir)/Domain_Error_Exception.java \
$(srcdir)/Generator.java \
$(srcdir)/Generator_System.java \
$(srcdir)/Generator_Type.java \
$(srcdir)/Grid_Generator.java \
$(srcdir)/Grid_Generator_System.java \
$(srcdir)/Grid_Generator_Type.java \
$(srcdir)/Invalid_Argument_Exception.java \
$(srcdir)/IO.java \
$(srcdir)/Length_Error_Exception.java \
$(srcdir)/Linear_Expression.java \
$(srcdir)/Linear_Expression_Coefficient.java \
$(srcdir)/Linear_Expression_Difference.java \
$(srcdir)/Linear_Expression_Sum.java \
$(srcdir)/Linear_Expression_Times.java \
$(srcdir)/Linear_Expression_Unary_Minus.java \
$(srcdir)/Linear_Expression_Variable.java \
$(srcdir)/Logic_Error_Exception.java \
$(srcdir)/MIP_Problem.java \
$(srcdir)/MIP_Problem_Status.java \
$(srcdir)/Optimization_Mode.java \
$(srcdir)/Overflow_Error_Exception.java \
$(srcdir)/Pair.java \
$(srcdir)/Parma_Polyhedra_Library.java \
$(srcdir)/Partial_Function.java \
$(srcdir)/Poly_Con_Relation.java \
$(srcdir)/Poly_Gen_Relation.java \
$(srcdir)/PPL_Object.java \
$(srcdir)/Relation_Symbol.java \
$(srcdir)/Variable.java \
$(srcdir)/Variables_Set.java

# NOTE: do _NOT_ add Fake_Class_for_Doxygen.java to this list.
all_java_sources = \
$(fixed_java_sources) \
$(required_instantiations_java_sources)

# NOTE: Fake_Class_for_Doxygen.java should be added here.
EXTRA_DIST = \
$(interface_generator_files) \
$(fixed_java_sources) \
$(srcdir)/Fake_Class_for_Doxygen.java

interface_generator_dependencies = \
$(top_builddir)/interfaces/ppl_interface_instantiations.m4 \
../../ppl_interface_generator_common.m4 \
../../ppl_interface_generator_common_dat.m4 \
../../ppl_interface_generator_copyright \
../../ppl_interface_generator_common_procedure_generators.m4 \
../ppl_interface_generator_java_procedure_generators.m4 \
$(interface_generator_files)

BUILT_SOURCES = \
java_classes.stamp \
java_cxx_headers.stamp

java_classes.stamp: $(all_java_sources)
	$(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) \
		$(AM_JAVACFLAGS) $(JAVACFLAGS) \
		$(all_java_sources)
	echo timestamp >$@

java_cxx_headers.stamp: java_classes.stamp
	$(JAVAH) -classpath .. \
		$(fixed_java_cxx_headers) \
		$(required_instantiations_java_cxx_headers)
	echo timestamp >$@

source_generation.stamp: $(interface_generator_dependencies)
	$(M4) --prefix-builtin -I../.. \
		-I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../.. \
		$(srcdir)/ppl_interface_generator_java_classes_java.m4 \
			> java_blob
	$(top_srcdir)/utils/cm_cleaner.sh < java_blob
	$(top_srcdir)/utils/cm_splitter.sh < java_blob
	rm -f java_blob
	echo timestamp >$@

$(required_instantiations_java_sources): source_generation.stamp

CLEANFILES = \
$(required_instantiations_java_sources) \
$(required_instantiations_java_classes) \
$(required_instantiations_java_cxx_headers_sources) \
$(fixed_java_cxx_headers_sources) \
java_classes.stamp \
java_cxx_headers.stamp \
source_generation.stamp \
By_Reference.class \
Coefficient.class \
Congruence.class \
Complexity_Class.class \
Congruence_System.class \
Constraint.class \
Constraint_System.class \
Constraint_System.class \
Control_Parameter_Name.class \
Control_Parameter_Value.class \
Degenerate_Element.class \
Domain_Error_Exception.class \
Generator.class \
Generator_System.class \
Generator_Type.class \
Grid_Generator.class \
Grid_Generator_System.class \
Grid_Generator_Type.class \
Invalid_Argument_Exception.class \
IO.class \
Length_Error_Exception.class \
Linear_Expression.class \
Linear_Expression_Coefficient.class \
Linear_Expression_Difference.class \
Linear_Expression_Sum.class \
Linear_Expression_Times.class \
Linear_Expression_Unary_Minus.class \
Linear_Expression_Variable.class \
Logic_Error_Exception.class \
MIP_Problem.class \
MIP_Problem_Status.class \
Optimization_Mode.class \
Overflow_Error_Exception.class \
PPL_Object.class \
Parma_Polyhedra_Library.class \
Pair.class \
Partial_Function.class \
Poly_Con_Relation.class \
Poly_Gen_Relation.class \
Relation_Symbol.class \
Variable.class \
Variables_Set.class

$(top_builddir)/interfaces/ppl_interface_instantiations.m4:
	$(MAKE) $(AM_MAKEFLAGS) -C $(top_builddir)/interfaces ppl_interface_instantiations.m4
